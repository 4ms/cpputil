#include "../util/compact_binary_writer.hh"
#include "../util/compact_binary_parser.hh"
#include "doctest.h"

enum class WKeys : uint16_t {
	BPM = 0x55,
	Track = 0xA0,
	Mode = 0xA2,
	Length = 0xA3,
};

// Compile-time: single field round-trip
static constexpr auto test_writer_single() {
	std::array<uint8_t, 8> buf{};
	CompactBinaryWriter<WKeys> w{buf};
	w.field(WKeys::BPM, uint32_t(120));
	return CompactBinaryParser<WKeys>{buf}.get<uint32_t>(WKeys::BPM);
}
static_assert(test_writer_single().value() == 120);

// Compile-time: size() returns correct value
static constexpr auto test_writer_size() {
	std::array<uint8_t, 64> buf{};
	CompactBinaryWriter<WKeys> w{buf};
	w.field(WKeys::BPM, uint32_t(120));
	w.field(WKeys::Mode, uint16_t(45));
	return w.size();
}
static_assert(test_writer_size() == 14);

// Compile-time: sub-object round-trip
static constexpr auto test_writer_sub_object() {
	std::array<uint8_t, 24> buf{};
	CompactBinaryWriter<WKeys> w{buf};
	w.field(WKeys::BPM, uint32_t(120));
	w.begin_object(WKeys::Track);
	w.field(WKeys::BPM, uint16_t(180));
	w.field(WKeys::Mode, uint16_t(4));
	w.end_object();

	auto p = CompactBinaryParser<WKeys>{buf};
	auto track = p.get_node(WKeys::Track);
	return std::pair{
		CompactBinaryParser<WKeys>{track}.get<uint16_t>(WKeys::BPM),
		CompactBinaryParser<WKeys>{track}.get<uint16_t>(WKeys::Mode),
	};
}
static_assert(test_writer_sub_object().first.value() == 180);
static_assert(test_writer_sub_object().second.value() == 4);

TEST_CASE("CompactBinaryWriter") {
	using Writer = CompactBinaryWriter<WKeys>;
	using Parser = CompactBinaryParser<WKeys>;

	SUBCASE("Single field round-trip") {
		std::array<uint8_t, 8> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		CHECK(w.size() == 8);
		CHECK(Parser{buf}.get<uint32_t>(WKeys::BPM).value() == 120);
	}

	SUBCASE("Multiple fields round-trip") {
		std::array<uint8_t, 64> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		w.field(WKeys::Mode, uint16_t(45));
		CHECK(w.size() == 14);

		auto p = Parser{std::span{buf}.first(w.size())};
		CHECK(p.get<uint32_t>(WKeys::BPM).value() == 120);
		CHECK(p.get<uint16_t>(WKeys::Mode).value() == 45);
	}

	SUBCASE("Sub-object round-trip") {
		std::array<uint8_t, 64> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		w.begin_object(WKeys::Track);
		w.field(WKeys::BPM, uint16_t(180));
		w.field(WKeys::Mode, uint16_t(4));
		w.end_object();

		auto p = Parser{std::span{buf}.first(w.size())};
		CHECK(p.get<uint32_t>(WKeys::BPM).value() == 120);

		auto track = p.get_node(WKeys::Track);
		CHECK(track.size() == 12);
		CHECK(Parser{track}.get<uint16_t>(WKeys::BPM).value() == 180);
		CHECK(Parser{track}.get<uint16_t>(WKeys::Mode).value() == 4);
	}

	SUBCASE("Deeply nested sub-objects") {
		std::array<uint8_t, 128> buf{};
		Writer w{buf};
		w.begin_object(WKeys::Track);
		w.field(WKeys::Length, uint16_t(8));
		w.begin_object(WKeys::Mode);
		w.field(WKeys::BPM, uint8_t(99));
		w.end_object();
		w.end_object();

		auto track = Parser{std::span{buf}.first(w.size())}.get_node(WKeys::Track);
		CHECK(Parser{track}.get<uint16_t>(WKeys::Length).value() == 8);

		auto mode = Parser{track}.get_node(WKeys::Mode);
		CHECK(Parser{mode}.get<uint8_t>(WKeys::BPM).value() == 99);
	}

	SUBCASE("Fields after sub-object") {
		std::array<uint8_t, 64> buf{};
		Writer w{buf};
		w.begin_object(WKeys::Track);
		w.field(WKeys::BPM, uint16_t(180));
		w.end_object();
		w.field(WKeys::Mode, uint16_t(45));

		auto p = Parser{std::span{buf}.first(w.size())};
		auto track = p.get_node(WKeys::Track);
		CHECK(Parser{track}.get<uint16_t>(WKeys::BPM).value() == 180);
		CHECK(p.get<uint16_t>(WKeys::Mode).value() == 45);
	}

	SUBCASE("Overflow returns total bytes needed") {
		std::array<uint8_t, 4> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		CHECK(w.size() == 8);
		CHECK(w.size() > buf.size());
	}

	SUBCASE("raw() bytes") {
		std::array<uint8_t, 64> buf{};
		Writer w{buf};
		std::array<uint8_t, 3> raw_data{10, 20, 30};
		w.raw(WKeys::BPM, raw_data);
		CHECK(w.size() == 7);
		auto node = Parser{std::span{buf}.first(w.size())}.get_node(WKeys::BPM);
		CHECK(node.size() == 3);
		CHECK(node[0] == 10);
		CHECK(node[1] == 20);
		CHECK(node[2] == 30);
	}

	SUBCASE("Matches hand-crafted blob from parser tests") {
		std::array<uint8_t, 14> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		w.field(WKeys::Mode, uint16_t(45));

		std::array<uint8_t, 14> expected{
			0x55, 0x00, 0x04, 0x00, 120, 0x00, 0x00, 0x00, 0xA2, 0x00, 0x02, 0x00, 45, 0x00};
		CHECK(buf == expected);
	}

	SUBCASE("Matches hand-crafted sub-object blob from parser tests") {
		std::array<uint8_t, 24> buf{};
		Writer w{buf};
		w.field(WKeys::BPM, uint32_t(120));
		w.begin_object(WKeys::Track);
		w.field(WKeys::BPM, uint16_t(180));
		w.field(WKeys::Mode, uint16_t(4));
		w.end_object();

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

	SUBCASE("end_object with no matching begin is safe") {
		std::array<uint8_t, 64> buf{};
		Writer w{buf};
		w.end_object(); // should be a no-op
		w.field(WKeys::BPM, uint32_t(120));
		CHECK(w.size() == 8);
		CHECK(Parser{std::span{buf}.first(w.size())}.get<uint32_t>(WKeys::BPM).value() == 120);
	}
}

TEST_CASE("CompactBinaryWriter with char keys") {
	using Writer = CompactBinaryWriter<const char[3], uint8_t>;
	using Parser = CompactBinaryParser<const char[3], uint8_t>;

	SUBCASE("Full-length key") {
		std::array<uint8_t, 16> buf{};
		Writer w{buf};
		w.field("bpm", uint8_t(120));
		CHECK(w.size() == 5);
		CHECK(Parser{std::span{buf}.first(w.size())}.get<uint8_t>("bpm").value() == 120);
	}

	SUBCASE("Short key with zero-padding") {
		std::array<uint8_t, 16> buf{};
		Writer w{buf};
		w.field("x", uint8_t(55));
		CHECK(w.size() == 5);
		CHECK(Parser{std::span{buf}.first(w.size())}.get<uint8_t>("x").value() == 55);
	}

	SUBCASE("Multiple char keys") {
		std::array<uint8_t, 32> buf{};
		Writer w{buf};
		w.field("bpm", uint8_t(120));
		w.field("t1", uint8_t(5));
		w.field("x", uint8_t(55));

		auto p = Parser{std::span{buf}.first(w.size())};
		CHECK(p.get<uint8_t>("bpm").value() == 120);
		CHECK(p.get<uint8_t>("t1").value() == 5);
		CHECK(p.get<uint8_t>("x").value() == 55);
	}

	SUBCASE("Matches hand-crafted char blob from parser tests") {
		std::array<uint8_t, 15> buf{};
		Writer w{buf};
		w.field("bpm", uint8_t(120));
		w.field("t1", uint8_t(5));
		w.field("x", uint8_t(55));

		std::array<uint8_t, 15> expected = {
			'b', 'p', 'm', 0x1, 120, 't', '1', '\0', 0x1, 5, 'x', '\0', '\0', 0x1, 55};
		CHECK(buf == expected);
	}
}

TEST_CASE("CompactBinaryWriter with uint8_t keys") {
	using Writer = CompactBinaryWriter<uint8_t, uint8_t>;
	using Parser = CompactBinaryParser<uint8_t, uint8_t>;

	SUBCASE("Minimal header size") {
		std::array<uint8_t, 4> buf{};
		Writer w{buf};
		w.field(uint8_t(1), uint16_t(0x1234));
		CHECK(w.size() == 4);
		CHECK(Parser{buf}.get<uint16_t>(1).value() == 0x1234);
	}
}
