#include "../util/compact_binary_serializer.hh"
#include "../util/compact_binary_parser.hh"
#include "doctest.h"

enum class SKeys : uint16_t {
	BPM = 0x55,
	Track = 0xA0,
	Mode = 0xA2,
	Length = 0xA3,
};

// Compile-time: serialize a single field and parse it back
static constexpr auto test_single_field() {
	std::array<uint8_t, 8> buf{};
	CompactBinarySerializer<SKeys>::serialize(buf, [](auto &s) { s.field(SKeys::BPM, uint32_t(120)); });
	return CompactBinaryParser<SKeys>{buf}.get<uint32_t>(SKeys::BPM);
}
static_assert(test_single_field().value() == 120);

// Compile-time: measure returns correct size
static constexpr auto test_measure() {
	return CompactBinarySerializer<SKeys>::measure([](auto &s) { s.field(SKeys::BPM, uint32_t(120)); });
}
static_assert(test_measure() == 8);

// Compile-time: multiple fields
static constexpr auto test_multiple_fields() {
	std::array<uint8_t, 14> buf{};
	CompactBinarySerializer<SKeys>::serialize(buf, [](auto &s) {
		s.field(SKeys::BPM, uint32_t(120));
		s.field(SKeys::Mode, uint16_t(45));
	});
	return std::pair{
		CompactBinaryParser<SKeys>{buf}.get<uint32_t>(SKeys::BPM),
		CompactBinaryParser<SKeys>{buf}.get<uint16_t>(SKeys::Mode),
	};
}
static_assert(test_multiple_fields().first.value() == 120);
static_assert(test_multiple_fields().second.value() == 45);

// Compile-time: sub-objects
static constexpr auto test_sub_object() {
	std::array<uint8_t, 24> buf{};
	CompactBinarySerializer<SKeys>::serialize(buf, [](auto &s) {
		s.field(SKeys::BPM, uint32_t(120));
		s.object(SKeys::Track, [](auto &s) {
			s.field(SKeys::BPM, uint16_t(180));
			s.field(SKeys::Mode, uint16_t(4));
		});
	});
	auto p = CompactBinaryParser<SKeys>{buf};
	auto track = p.get_node(SKeys::Track);
	return std::pair{
		CompactBinaryParser<SKeys>{track}.get<uint16_t>(SKeys::BPM),
		CompactBinaryParser<SKeys>{track}.get<uint16_t>(SKeys::Mode),
	};
}
static_assert(test_sub_object().first.value() == 180);
static_assert(test_sub_object().second.value() == 4);

TEST_CASE("CompactBinarySerializer") {
	using Ser = CompactBinarySerializer<SKeys>;
	using Parser = CompactBinaryParser<SKeys>;

	SUBCASE("Single field round-trip") {
		std::array<uint8_t, 8> buf{};
		auto n = Ser::serialize(buf, [](auto &s) { s.field(SKeys::BPM, uint32_t(120)); });
		CHECK(n == 8);
		CHECK(Parser{buf}.get<uint32_t>(SKeys::BPM).value() == 120);
	}

	SUBCASE("Multiple fields round-trip") {
		std::array<uint8_t, 64> buf{};
		auto n = Ser::serialize(buf, [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.field(SKeys::Mode, uint16_t(45));
		});
		CHECK(n == 14);
		auto p = Parser{std::span{buf}.first(n)};
		CHECK(p.get<uint32_t>(SKeys::BPM).value() == 120);
		CHECK(p.get<uint16_t>(SKeys::Mode).value() == 45);
	}

	SUBCASE("Sub-object round-trip") {
		std::array<uint8_t, 64> buf{};
		auto n = Ser::serialize(buf, [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.object(SKeys::Track, [](auto &s) {
				s.field(SKeys::BPM, uint16_t(180));
				s.field(SKeys::Mode, uint16_t(4));
			});
		});

		auto p = Parser{std::span{buf}.first(n)};
		CHECK(p.get<uint32_t>(SKeys::BPM).value() == 120);

		auto track = p.get_node(SKeys::Track);
		CHECK(track.size() == 12);
		CHECK(Parser{track}.get<uint16_t>(SKeys::BPM).value() == 180);
		CHECK(Parser{track}.get<uint16_t>(SKeys::Mode).value() == 4);
	}

	SUBCASE("Deeply nested sub-objects") {
		std::array<uint8_t, 128> buf{};
		auto n = Ser::serialize(buf, [](auto &s) {
			s.object(SKeys::Track, [](auto &s) {
				s.field(SKeys::Length, uint16_t(8));
				s.object(SKeys::Mode, [](auto &s) { s.field(SKeys::BPM, uint8_t(99)); });
			});
		});

		auto track = Parser{std::span{buf}.first(n)}.get_node(SKeys::Track);
		CHECK(Parser{track}.get<uint16_t>(SKeys::Length).value() == 8);

		auto mode = Parser{track}.get_node(SKeys::Mode);
		CHECK(Parser{mode}.get<uint8_t>(SKeys::BPM).value() == 99);
	}

	SUBCASE("measure() matches serialize()") {
		auto describe = [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.field(SKeys::Mode, uint16_t(45));
		};
		auto m = Ser::measure(describe);
		std::array<uint8_t, 64> buf{};
		auto n = Ser::serialize(buf, describe);
		CHECK(m == n);
		CHECK(m == 14);
	}

	SUBCASE("measure() with sub-objects") {
		auto describe = [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.object(SKeys::Track, [](auto &s) {
				s.field(SKeys::BPM, uint16_t(180));
				s.field(SKeys::Mode, uint16_t(4));
			});
		};
		CHECK(Ser::measure(describe) == 24);
	}

	SUBCASE("Overflow returns total bytes needed") {
		std::array<uint8_t, 4> buf{};
		auto n = Ser::serialize(buf, [](auto &s) { s.field(SKeys::BPM, uint32_t(120)); });
		CHECK(n == 8);
		CHECK(n > buf.size());
	}

	SUBCASE("stream() to accumulator") {
		std::array<uint8_t, 64> accumulated{};
		size_t acc_pos = 0;
		auto sink = [&](std::span<const uint8_t> chunk) {
			for (auto b : chunk)
				accumulated[acc_pos++] = b;
		};

		auto describe = [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.field(SKeys::Mode, uint16_t(45));
		};
		auto n = Ser::stream(describe, sink);
		CHECK(n == 14);
		CHECK(acc_pos == 14);

		auto p = Parser{std::span{accumulated}.first(n)};
		CHECK(p.get<uint32_t>(SKeys::BPM).value() == 120);
		CHECK(p.get<uint16_t>(SKeys::Mode).value() == 45);
	}

	SUBCASE("raw() bytes") {
		std::array<uint8_t, 64> buf{};
		std::array<uint8_t, 3> raw_data{10, 20, 30};
		auto n = Ser::serialize(buf, [&](auto &s) { s.raw(SKeys::BPM, raw_data); });
		CHECK(n == 7);
		auto node = Parser{std::span{buf}.first(n)}.get_node(SKeys::BPM);
		CHECK(node.size() == 3);
		CHECK(node[0] == 10);
		CHECK(node[1] == 20);
		CHECK(node[2] == 30);
	}

	SUBCASE("Matches hand-crafted blob from parser tests") {
		std::array<uint8_t, 14> buf{};
		Ser::serialize(buf, [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.field(SKeys::Mode, uint16_t(45));
		});
		std::array<uint8_t, 14> expected{
			0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xA2, 0x00, 0x02, 0x00, 45, 0x00};
		CHECK(buf == expected);
	}

	SUBCASE("Matches hand-crafted sub-object blob from parser tests") {
		std::array<uint8_t, 24> buf{};
		Ser::serialize(buf, [](auto &s) {
			s.field(SKeys::BPM, uint32_t(120));
			s.object(SKeys::Track, [](auto &s) {
				s.field(SKeys::BPM, uint16_t(180));
				s.field(SKeys::Mode, uint16_t(4));
			});
		});
		// clang-format off
		std::array<uint8_t, 24> expected{
			0x55, 0x00, 0x04, 0x00,
			120, 0x00, 0x00, 0x00,

			0xA0, 0x00, 0x0C, 0x00,
				0x55, 0x00, 0x02, 0x00,
				180, 0x00,
				0xA2, 0x00, 0x02, 0x00,
				4, 0x00,
		};
		// clang-format on
		CHECK(buf == expected);
	}
}

TEST_CASE("CompactBinarySerializer with char keys") {
	using Ser = CompactBinarySerializer<const char[3], uint8_t>;
	using Parser = CompactBinaryParser<const char[3], uint8_t>;

	SUBCASE("Full-length key") {
		std::array<uint8_t, 16> buf{};
		auto n = Ser::serialize(buf, [](auto &s) { s.field("bpm", uint8_t(120)); });
		CHECK(n == 5);
		CHECK(Parser{std::span{buf}.first(n)}.get<uint8_t>("bpm").value() == 120);
	}

	SUBCASE("Short key with zero-padding") {
		std::array<uint8_t, 16> buf{};
		auto n = Ser::serialize(buf, [](auto &s) { s.field("x", uint8_t(55)); });
		CHECK(n == 5);
		CHECK(Parser{std::span{buf}.first(n)}.get<uint8_t>("x").value() == 55);
	}

	SUBCASE("Multiple char keys") {
		std::array<uint8_t, 32> buf{};
		auto n = Ser::serialize(buf, [](auto &s) {
			s.field("bpm", uint8_t(120));
			s.field("t1", uint8_t(5));
			s.field("x", uint8_t(55));
		});
		auto p = Parser{std::span{buf}.first(n)};
		CHECK(p.get<uint8_t>("bpm").value() == 120);
		CHECK(p.get<uint8_t>("t1").value() == 5);
		CHECK(p.get<uint8_t>("x").value() == 55);
	}

	SUBCASE("Matches hand-crafted char blob from parser tests") {
		std::array<uint8_t, 15> buf{};
		Ser::serialize(buf, [](auto &s) {
			s.field("bpm", uint8_t(120));
			s.field("t1", uint8_t(5));
			s.field("x", uint8_t(55));
		});
		std::array<uint8_t, 15> expected = {
			'b', 'p', 'm', 0x1, 120, 't', '1', '\0', 0x1, 5, 'x', '\0', '\0', 0x1, 55};
		CHECK(buf == expected);
	}
}

TEST_CASE("CompactBinarySerializer with uint8_t keys") {
	using Ser = CompactBinarySerializer<uint8_t, uint8_t>;
	using Parser = CompactBinaryParser<uint8_t, uint8_t>;

	SUBCASE("Minimal header size") {
		std::array<uint8_t, 4> buf{};
		auto n = Ser::serialize(buf, [](auto &s) { s.field(uint8_t(1), uint16_t(0x1234)); });
		CHECK(n == 4);
		CHECK(Parser{buf}.get<uint16_t>(1).value() == 0x1234);
	}
}
