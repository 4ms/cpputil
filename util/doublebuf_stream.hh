#pragma once
#include "util/callable.hh"
#include "util/fixed_vector.hh"
#include <optional>
#include <span>

// A double-buffered data transmitter.
// Allows for calling transmit() at any time, even if a transmission is in progress.
// Not thread-safe (would need to use std::atomic or semaphors to make that work).
//
// Call transmit() to send data using the transmit_func provided in the constructor.
// When transmission is complete (hardware is ready to send more), then call tx_done_callback().
//
// When you call transmit():
//  -- if a transmission is in progress, then the data will be queued into the inactive buffer
//  -- if a transmission is NOT in progress, then the data will be sent immediately.
//
// When a transmission ends (tx_done_callback is called),
// if the inactive buffer has data, then a new transmission will be started with it.

// Note: This is not safe if tx_done_callback() interrupts transmit() or start_tx().
// A valid use case is in the STM USB host library, which does not call
// the tx callback in an IRQ, so we do not run the risk of tx_done_callback()
// interrupting transmit() or start_tx();

// #define DEBUG_DBLBUFSTREAM 1
#if DEBUG_DBLBUFSTREAM
#define dbs_pr_err printf
#else
#define dbs_pr_err(...)
#endif

template<typename T, size_t Size>
struct DoubleBufferStream {
	// This size can be adjusted if needed
	FixedVector<T, Size> tx_buffer[2];
	std::optional<unsigned> in_progress_idx = std::nullopt;

	Function<bool(std::span<const T>)> transmit_func;

	DoubleBufferStream()
		: transmit_func{[](std::span<const T>) { return false; }} {
	}

	DoubleBufferStream(auto xmit_func)
		: transmit_func{xmit_func} {
	}

	void set_transmit_func(auto xmit_func) {
		transmit_func = std::move(xmit_func);
	}

	// Pushes to the inactive buffer and starts transmission if it's not already started
	bool transmit(std::span<T> word) {
		auto inactive_idx = 1 - in_progress_idx.value_or(1);
		auto &inactive_buf = tx_buffer[inactive_idx];

		if (inactive_buf.available() >= word.size()) {
			for (auto d : word)
				inactive_buf.push_back(d);

			// pr_dbg("DBS::transmit: queued tx data in buffer %u\n", inactive_idx);

			// Start a new transmission if one isn't in progress
			if (!in_progress_idx.has_value()) {
				// pr_dbg("DBS::transmit(): no tx in progress, so starting now\n");
				return start_tx(inactive_idx);
			}

			// tx_done_callback() will transmit our data when it's called
			// pr_dbg("DBS::transmit(): tx is progress, so not starting new tx\n");
			return true;
		} else {
			return false;
		}
	}

	void tx_done_callback() {
		if (!in_progress_idx.has_value()) {
			dbs_pr_err("MIDI Host internal error: tx_done_callback called but no buffer is in progress\n");
			return;
		}

		tx_buffer[in_progress_idx.value()].clear();

		// pr_dbg("DBS::tx_done_callback(): finished tx of buffer[%u]\n", in_progress_idx.value());

		// Check if we should start transmitting the other buffer
		auto other_buffer = 1 - in_progress_idx.value();

		if (tx_buffer[other_buffer].size()) {
			// pr_dbg("DBS: tx_done_callback(): starting tx for buffer[%u]\n", other_buffer);
			start_tx(other_buffer);
		} else {
			in_progress_idx = std::nullopt;
		}
	}

private:
	bool start_tx(unsigned idx) {
		in_progress_idx = idx;
		// pr_dbg("DBS::start_tx(): starting tx of buffer[%u]\n", idx);
		auto &active_buf = tx_buffer[idx];
		auto res = transmit_func({active_buf.begin(), active_buf.size()});
		return res;
	}
};
