#include "../util/serial_parser.hh"
#include "doctest.h"

//
// Tests
//

enum class Keys : uint16_t {
	BPM = 0x55,
	NotFound = 0x66,
	Track = 0xA0,
	Mode = 0xA2,
};
static_assert(sizeof(Parser<Keys>::Header) == 4);

static constexpr std::array<const uint8_t, 8> blob{0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00};

static_assert(Parser<Keys>{blob}.get<uint32_t>(Keys::BPM).value_or(0) == 120);
static_assert(Parser<Keys>{blob}.get<uint16_t>(Keys::BPM) == std::nullopt);
static_assert(Parser<Keys>{blob}.get<uint32_t>(Keys::NotFound) == std::nullopt);

static constexpr std::array<const uint8_t, 14> blob2{
	0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xA2, 0x00, 0x02, 0x00, 45, 0x00};

static_assert(Parser<Keys>{blob2}.get<uint32_t>(Keys::BPM).value_or(0) == 120);
static_assert(Parser<Keys>{blob2}.get<uint16_t>(Keys::BPM) == std::nullopt);
static_assert(Parser<Keys>{blob2}.get<uint32_t>(Keys::NotFound) == std::nullopt);
static_assert(Parser<Keys>{blob2}.get<uint16_t>(Keys::Mode).value() == 45);

constexpr std::array<const uint8_t, 24> blob3{
	// clang-format off

	// BPM:
	0x55, 0x00, 0x04, 0x00, 
	120, 0x00, 0x00, 0x00,

	// Track (12 bytes)
	0xA0, 0x00, 0x0C, 0x00,
		// Track::BPM = 180
		0x55, 0x00, 0x02, 0x00, 
		180, 0x00,
		// Track::Mode = 4
		0xA2, 0x00, 0x02, 0x00, 
		4, 0x00,

	// clang-format on
};

constexpr auto track = Parser<Keys>{blob3}.get<std::array<const uint8_t, 12>>(Keys::Track);
static_assert(Parser<Keys>{*track}.get<uint16_t>(Keys::BPM) == 180);
static_assert(Parser<Keys>{*track}.get<uint16_t>(Keys::Mode) == 4);

TEST_CASE("Basic usage") {

	enum class Keys : uint16_t {
		BPM = 0x55,
		Track = 0x56,
		NotFound = 0xAA,
		Mode = 0xAB,
	};

	SUBCASE("Simply get a value") {
		constexpr std::array<const uint8_t, 8> blob{0x55, 0x00, 0x04, 0x00, 0x3F, 0x00, 0x00, 0x00};

		static_assert(Parser<Keys>{blob}.get<uint32_t>(Keys::BPM).value_or(0) == 63);

		auto bpm = Parser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(bpm.value() == 63);

		auto mode = Parser<Keys>{blob}.get<uint32_t>(Keys::Mode);
		CHECK_FALSE(mode.has_value());

		auto notfound = Parser<Keys>{blob}.get<uint16_t>(Keys::NotFound);
		CHECK_FALSE(notfound.has_value());
	}

	SUBCASE("Check unaligned data") {
		constexpr uint8_t PAD = 0xFF; //add a byte to force the data to be un-aligned

		alignas(4) std::array<const uint8_t, 15> blob{
			PAD, 0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xAB, 0x00, 0x02, 0x00, 45, 0x00};
		// blob_misalign is guarenteed to not be aligned to a 2 or 4 byte boundary
		auto blob_misalign = std::span{blob}.subspan(1);

		auto bpm = Parser<Keys>{blob_misalign}.get<uint32_t>(Keys::BPM);
		CHECK(bpm.value() == 120);

		auto mode = Parser<Keys>{blob_misalign}.get<uint16_t>(Keys::Mode);
		CHECK(mode.value() == 45);

		mode = Parser<Keys>{blob_misalign}.get<uint32_t>(Keys::Mode);
		CHECK_FALSE(mode.has_value());

		auto notfound = Parser<Keys>{blob_misalign}.get<uint16_t>(Keys::NotFound);
		CHECK_FALSE(notfound.has_value());
	}

	SUBCASE("Sub-objects") {
		std::array<const uint8_t, 24> blob{
			// clang-format off

			// BPM:
			0x55, 0x00, 0x04, 0x00, 
			120, 0x00, 0x00, 0x00,

			// Track (12 bytes)
			0x56, 0x00, 0x0C, 0x00,
				// Track::BPM = 180
				0x55, 0x00, 0x02, 0x00, 
				180, 0x00,
				// Track::Mode = 4
				0xAB, 0x00, 0x02, 0x00, 
				4, 0x00,

			// clang-format on
		};

		auto track = Parser<Keys>{blob}.get<std::array<const uint8_t, 12>>(Keys::Track);
		CHECK(track.has_value());

		auto track_bpm = Parser<Keys>{*track}.get<uint16_t>(Keys::BPM);
		CHECK(track_bpm.value() == 180);
	}
}
