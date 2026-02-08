#include "../util/compact_binary_parser.hh"
#include "../util/static_string.hh"
#include "doctest.h"

enum class Keys : uint16_t {
	BPM = 0x55,
	NotFound = 0x66,
	Track = 0xA0,
	Mode = 0xA2,
};
static_assert(sizeof(CompactBinaryParser<Keys>::Header) == 4);

static constexpr std::array<const uint8_t, 8> blob{0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00};

static_assert(CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM).value_or(0) == 120);
static_assert(CompactBinaryParser<Keys>{blob}.get<uint16_t>(Keys::BPM) == std::nullopt);
static_assert(CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::NotFound) == std::nullopt);

static constexpr std::array<const uint8_t, 14> blob2{
	0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xA2, 0x00, 0x02, 0x00, 45, 0x00};

static_assert(CompactBinaryParser<Keys>{blob2}.get<uint32_t>(Keys::BPM).value_or(0) == 120);
static_assert(CompactBinaryParser<Keys>{blob2}.get<uint16_t>(Keys::BPM) == std::nullopt);
static_assert(CompactBinaryParser<Keys>{blob2}.get<uint32_t>(Keys::NotFound) == std::nullopt);
static_assert(CompactBinaryParser<Keys>{blob2}.get<uint16_t>(Keys::Mode).value() == 45);

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

constexpr auto track = CompactBinaryParser<Keys>{blob3}.get<std::array<const uint8_t, 12>>(Keys::Track);
static_assert(CompactBinaryParser<Keys>{*track}.get<uint16_t>(Keys::BPM) == 180);
static_assert(CompactBinaryParser<Keys>{*track}.get<uint16_t>(Keys::Mode) == 4);

constexpr std::array<unsigned char, 5> blob_str = {'b', 'p', 'm', 0x1, 120};
static_assert(CompactBinaryParser<const char[3], uint8_t>{blob_str}.get<uint8_t>("bpm") == 120);

TEST_CASE("Basic usage") {

	enum class Keys : uint16_t {
		BPM = 0x55,
		Track = 0x56,
		NotFound = 0xAA,
		Mode = 0xAB,
	};

	SUBCASE("Simply get a value") {
		constexpr std::array<const uint8_t, 8> blob{0x55, 0x00, 0x04, 0x00, 0x3F, 0x00, 0x00, 0x00};

		static_assert(CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM).value_or(0) == 63);

		auto bpm = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(bpm.value() == 63);

		auto mode = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::Mode);
		CHECK_FALSE(mode.has_value());

		auto notfound = CompactBinaryParser<Keys>{blob}.get<uint16_t>(Keys::NotFound);
		CHECK_FALSE(notfound.has_value());
	}

	SUBCASE("Check unaligned data") {
		constexpr uint8_t PAD = 0xFF; //add a byte to force the data to be un-aligned

		alignas(4) std::array<const uint8_t, 15> blob{
			PAD, 0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xAB, 0x00, 0x02, 0x00, 45, 0x00};
		// blob_misalign is guaranteed to not be aligned to a 2 or 4 byte boundary
		auto blob_misalign = std::span{blob}.subspan(1);

		auto bpm = CompactBinaryParser<Keys>{blob_misalign}.get<uint32_t>(Keys::BPM);
		CHECK(bpm.value() == 120);

		auto mode = CompactBinaryParser<Keys>{blob_misalign}.get<uint16_t>(Keys::Mode);
		CHECK(mode.value() == 45);

		mode = CompactBinaryParser<Keys>{blob_misalign}.get<uint32_t>(Keys::Mode);
		CHECK_FALSE(mode.has_value());

		auto notfound = CompactBinaryParser<Keys>{blob_misalign}.get<uint16_t>(Keys::NotFound);
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

		auto track = CompactBinaryParser<Keys>{blob}.get<std::array<const uint8_t, 12>>(Keys::Track);
		CHECK(track.has_value());

		auto track_bpm = CompactBinaryParser<Keys>{*track}.get<uint16_t>(Keys::BPM);
		CHECK(track_bpm.value() == 180);
	}

	SUBCASE("Check bad data size") {

		std::array<const uint8_t, 4> blob{0x55, 0x00, 0xFF, 0x00};

		auto bpm = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(!bpm.has_value());
	}

	SUBCASE("Check header too small ") {

		std::array<const uint8_t, 3> blob{0x55, 0x00, 0x04};

		auto bpm = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(!bpm.has_value());
	}

	SUBCASE("Check missing data") {

		std::array<const uint8_t, 5> blob{0x55, 0x00, 0x04, 0x00, 0x01};

		auto bpm = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(!bpm.has_value());
	}

	SUBCASE("ASCII keys with StaticString") {
		std::array<unsigned char, 6> blob_str = {'b', 'p', 'm', 0, 0x1, 120};
		auto p = CompactBinaryParser<StaticString<3>, uint8_t>{blob_str};
		auto bpm = p.get<uint8_t>(StaticString<3>{"bpm"});
		CHECK(bpm.has_value());
		CHECK(bpm.value() == 120);
	}

	SUBCASE("ASCII keys with char array") {
		std::array<unsigned char, 5> blob_str = {'b', 'p', 'm', 0x1, 120};
		static_assert(sizeof(CompactBinaryParser<const char[3]>::Header) == 5);
		static_assert(sizeof(CompactBinaryParser<const char[2]>::Header) == 4);

		auto p = CompactBinaryParser<const char[3], uint8_t>{blob_str};
		auto bpm = p.get<uint8_t>("bpm");
		CHECK(bpm.has_value());
		CHECK(bpm.value() == 120);
	}

	SUBCASE("Zero-length data has no value even if key exists") {
		std::array<uint8_t, 2> blob{1, 0}; //key = 1, data-length = 0
		auto p = CompactBinaryParser<uint8_t, uint8_t>{blob};
		auto x = p.get<uint8_t>(1);
		CHECK(!x.has_value());
	}

	SUBCASE("Empty blob") {
		std::array<uint8_t, 0> blob{};
		auto p = CompactBinaryParser<uint8_t, uint8_t>{blob};
		auto x = p.get<uint8_t>(0);
		CHECK(!x.has_value());
	}

	SUBCASE("Multiple identical keys: return first found") {
		constexpr std::array<const uint8_t, 16> blob{
			// clang-format off
			0x55, 0x00, 0x04, 0x00, 50, 0x00, 0x00, 0x00,
			0x55, 0x00, 0x04, 0x00, 100, 0x00, 0x00, 0x00,
			// clang-format on
		};

		auto bpm = CompactBinaryParser<Keys>{blob}.get<uint32_t>(Keys::BPM);
		CHECK(bpm.value() == 50);
	}
}
