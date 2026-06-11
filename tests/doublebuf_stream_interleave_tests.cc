// Interleaving simulator for the USB MIDI host TX path:
//   parse_midi (FrameRate ISR) -> MidiHost::transmit -> DoubleBufferStream
//   -> USBH_MIDI_Transmit -> MIDI_ProcessTransmission (main loop) -> URB
//
// Models every statement of DoubleBufferStream::transmit()/tx_done_callback()/
// start_tx() and of the usbh_midi TX state machine as one "micro-step" of the
// main loop. The ISR (transmit of one 4-byte packet) runs atomically and can
// be scheduled between any two micro-steps -- exactly the preemption the real
// FrameRate ISR can do to USBH_Process. The URB completes a configurable
// number of steps after submission (submission copies the bytes, mirroring
// the synchronous FIFO write in slave mode).
//
// Invariant checked: every byte accepted by transmit() appears on the wire
// exactly once, in order. The hardware shows ~1e-4 of trailing message chunks
// being appended and then cleared without ever being submitted; this test
// searches the schedule space for such a loss.
//
// Code mirrored from lib/cpputil/util/doublebuf_stream.hh and
// src/usb/usbh_midi.cc (MIDI_ProcessTransmission / USBH_MIDI_Transmit).

#include "doctest.h"
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace
{

constexpr size_t BufSize = 1024;

// FixedVector<uint8_t, BufSize> stand-in (data + back_idx, stable storage)
struct FixedBuf {
	std::array<uint8_t, BufSize> d{};
	size_t len = 0;

	size_t available() const {
		return BufSize - len;
	}
	void push_back(uint8_t b) {
		if (len < BufSize)
			d[len++] = b;
	}
	void clear() {
		len = 0;
	}
	size_t size() const {
		return len;
	}
	const uint8_t *data() const {
		return d.data();
	}
};

struct Sim {
	// ---- DoubleBufferStream state ----
	FixedBuf tx_buffer[2];
	std::optional<unsigned> in_progress_idx = std::nullopt;

	// ---- usbh_midi MSHandle TX state ----
	enum class TxState { Idle, SendData, SendDataWait };
	TxState data_tx_state = TxState::Idle;
	const uint8_t *pTxData = nullptr;
	uint32_t TxDataLength = 0;

	// ---- URB / wire model ----
	enum class Urb { Idle, InFlight, Done };
	Urb urb = Urb::Idle;
	int urb_countdown = 0;
	int urb_latency = 3;
	std::vector<uint8_t> urb_data;
	std::vector<uint8_t> wire;

	// ---- bookkeeping ----
	std::vector<uint8_t> accepted; // bytes transmit() accepted, in order
	uint32_t drops = 0;
	std::vector<std::string> trace;
	bool tracing = false;

	void log(const char *fmt, ...) {
		if (!tracing)
			return;
		char buf[160];
		va_list va;
		va_start(va, fmt);
		vsnprintf(buf, sizeof(buf), fmt, va);
		va_end(va);
		char state[160];
		snprintf(state,
				 sizeof(state),
				 " [pc=%d inprog=%d buf0=%zu buf1=%zu txstate=%d urb=%d]",
				 pc,
				 in_progress_idx ? (int)*in_progress_idx : -1,
				 tx_buffer[0].size(),
				 tx_buffer[1].size(),
				 (int)data_tx_state,
				 (int)urb);
		trace.push_back(std::string(buf) + state);
	}

	// ======== ISR: MidiHost::transmit -> DBS::transmit (runs atomically) ====
	void isr_transmit(const std::array<uint8_t, 4> &pkt) {
		// DBS::transmit()
		auto inactive_idx = 1 - in_progress_idx.value_or(1);
		auto &inactive_buf = tx_buffer[inactive_idx];

		if (inactive_buf.available() >= pkt.size()) {
			for (auto b : pkt) {
				inactive_buf.push_back(b);
				accepted.push_back(b);
			}
			bool started = false;
			if (!in_progress_idx.has_value()) {
				start_tx(inactive_idx);
				started = true;
			}
			log("ISR pkt %02x.. -> buf%u%s", pkt[0], inactive_idx, started ? " +start_tx" : "");
		} else {
			drops++;
		}
	}

	// DBS::start_tx -- called from ISR (atomic) or from main loop (split into
	// micro-steps below; this whole-function version is only used by the ISR,
	// which cannot be preempted)
	void start_tx(unsigned idx) {
		in_progress_idx = idx;
		auto &active_buf = tx_buffer[idx];
		usbh_midi_transmit(active_buf.data(), active_buf.size());
	}

	// USBH_MIDI_Transmit (state guard always passes in steady state)
	void usbh_midi_transmit(const uint8_t *pbuff, uint32_t length) {
		pTxData = pbuff;
		TxDataLength = length;
		data_tx_state = TxState::SendData;
	}

	// ======== main loop: one micro-step per call =========
	// pc encodes where we are inside USBH_Process/MIDI_ProcessTransmission/
	// tx_done_callback. The ISR may fire between any two calls of main_step().
	int pc = 0;
	unsigned td_other = 0; // tx_done_callback local: other_buffer

	void main_step() {
		switch (pc) {
			case 0: // top of MIDI_ProcessTransmission
				if (data_tx_state == TxState::SendData)
					pc = 10;
				else if (data_tx_state == TxState::SendDataWait)
					pc = 20;
				// else Idle: stay
				break;

			case 10: // SendData: USBH_BulkSendData -- copies bytes (FIFO write)
				urb_data.assign(pTxData, pTxData + TxDataLength);
				urb = Urb::InFlight;
				urb_countdown = urb_latency;
				log("submit URB len=%u first=%02x", TxDataLength, TxDataLength ? pTxData[0] : 0);
				pc = 11;
				break;

			case 11:
				data_tx_state = TxState::SendDataWait;
				pc = 0;
				break;

			case 20: // SendDataWait: poll URB state
				if (urb == Urb::Done) {
					log("URB done seen");
					pc = 21;
				} else {
					pc = 0;
				}
				break;

			case 21: // TxDataLength <= OutEP.size: TxDataLength = 0
				TxDataLength = 0;
				pc = 22;
				break;

			case 22: // data_tx_state = Idle
				data_tx_state = TxState::Idle;
				pc = 23;
				break;

			// ---- tx_done_callback, statement by statement ----
			case 23: // D1: if (!in_progress_idx) return
				pc = in_progress_idx.has_value() ? 24 : 0;
				break;

			case 24: // D2: tx_buffer[in_progress].clear()
				log("tx_done: clear buf%u (had %zu bytes)",
					in_progress_idx.value(),
					tx_buffer[in_progress_idx.value()].size());
				tx_buffer[in_progress_idx.value()].clear();
				pc = 25;
				break;

			case 25: // D3+D4: other = 1 - in_progress; if (other.size()) ...
				td_other = 1 - in_progress_idx.value();
				pc = (tx_buffer[td_other].size() != 0) ? 26 : 29;
				break;

			case 26: // start_tx S1: in_progress = idx
				log("tx_done: start_tx other");
				in_progress_idx = td_other;
				pc = 27;
				break;

			case 27: // start_tx S2 (span construction: data + size snapshot)
				pTxData = tx_buffer[td_other].data();
				TxDataLength = (uint32_t)tx_buffer[td_other].size();
				pc = 28;
				break;

			case 28: // USBH_MIDI_Transmit: state = SendData
				data_tx_state = TxState::SendData;
				pc = 0;
				break;

			case 29: // D6: in_progress = nullopt
				log("tx_done: idle (nullopt)");
				in_progress_idx = std::nullopt;
				pc = 0;
				break;
		}
	}

	// hardware: URB completes some steps after submission
	void hw_step() {
		if (urb == Urb::InFlight) {
			if (--urb_countdown <= 0) {
				wire.insert(wire.end(), urb_data.begin(), urb_data.end());
				urb = Urb::Done;
				log("URB complete (wire)");
			}
		}
	}
};

// Drive one scenario: bursts of 12 packets, `gap` steps between packets,
// `idle` steps between bursts, starting at phase `phase`.
// Returns true if byte conservation holds.
bool run_scenario(Sim &sim, int urb_latency, int gap, int idle, int phase, int bursts, std::string *fail_info = nullptr) {
	sim.urb_latency = urb_latency;

	uint8_t seq = 0;
	auto next_pkt = [&seq]() {
		std::array<uint8_t, 4> p{};
		for (auto &b : p)
			b = seq++;
		return p;
	};

	int step = 0;
	int burst = 0;
	int pkt_in_burst = 0;
	int next_fire = phase;

	const int burst_period = 12 * gap + idle;
	const int total_steps = phase + bursts * burst_period + 2000;

	while (step < total_steps) {
		if (step == next_fire && burst < bursts) {
			sim.isr_transmit(next_pkt());
			pkt_in_burst++;
			if (pkt_in_burst >= 12) {
				pkt_in_burst = 0;
				burst++;
				next_fire = step + idle;
			} else {
				next_fire = step + gap;
			}
		}
		sim.main_step();
		sim.hw_step();
		step++;
	}

	// drain: no more ISR traffic, let the main loop finish everything
	for (int i = 0; i < 5000; i++) {
		sim.main_step();
		sim.hw_step();
	}

	bool ok = (sim.wire == sim.accepted) && (sim.drops == 0);
	if (!ok && fail_info) {
		char buf[256];
		snprintf(buf,
				 sizeof(buf),
				 "lat=%d gap=%d idle=%d phase=%d: accepted=%zu wire=%zu drops=%u",
				 urb_latency,
				 gap,
				 idle,
				 phase,
				 sim.accepted.size(),
				 sim.wire.size(),
				 (unsigned)sim.drops);
		*fail_info = buf;

		// locate first divergence
		size_t n = std::min(sim.wire.size(), sim.accepted.size());
		for (size_t i = 0; i < n; i++) {
			if (sim.wire[i] != sim.accepted[i]) {
				snprintf(buf, sizeof(buf), " first divergence at byte %zu: accepted=%02x wire=%02x", i, sim.accepted[i], sim.wire[i]);
				*fail_info += buf;
				break;
			}
		}
	}
	return ok;
}

} // namespace

// Characterization of why the MIDI host TX path moved off DoubleBufferStream
// (see midi_host.hh, which now uses a LockFreeFifoSpsc + main-loop pump).
//
// This model executes the DBS statements in source order. Even so, the
// stranding window (ISR append between the D4 empty-check and D6 nullopt)
// produces out-of-order delivery in hundreds of schedules: that alone
// disqualifies the design for an ISR producer.
//
// The packet LOSS observed on hardware needs one more ingredient the model
// deliberately omits: the compiler. The -O3 ARM build of tx_done_callback
// reuses the D4 size-check load as the submitted span length and stores the
// new in_progress_idx afterwards, so an ISR transmit() landing between those
// two instructions appends to the buffer being submitted, beyond the captured
// length -- and completion-time clear() destroys the bytes. Source-order
// semantics (this model) cannot lose bytes; the compiled reordering can.
TEST_CASE("DoubleBufferStream + usbh_midi TX model: ISR producer reorders even in source order") {
	int losses = 0;	  // bytes accepted but never on the wire
	int reorders = 0; // all bytes delivered but out of order

	for (int lat : {1, 2, 3, 5, 8, 20}) {
		for (int gap : {1, 2, 3, 5, 9}) {
			for (int idle : {15, 30, 60}) {
				const int burst_period = 12 * gap + idle;
				for (int phase = 0; phase < burst_period; phase++) {
					Sim sim;
					if (!run_scenario(sim, lat, gap, idle, phase, 40)) {
						if (sim.wire.size() < sim.accepted.size())
							losses++;
						else
							reorders++;
					}
				}
			}
		}
	}

	// Source-order semantics: no loss, but plenty of reordering
	CHECK(losses == 0);
	CHECK(reorders > 0);
}
