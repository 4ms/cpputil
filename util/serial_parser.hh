#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <span>

template<typename Keys, typename DataSizeT = uint16_t>
struct Parser {
	struct Header {
		Keys key;
		DataSizeT size;
	};

	std::span<const uint8_t> blob;

	constexpr Parser(std::span<const uint8_t> blob)
		: blob{blob} {
	}

	template<typename T>
	constexpr static T as(std::span<const uint8_t> bytes) {
		// Copy to ensure alignment:
		alignas(T) std::array<std::byte, sizeof(T)> buffer{};

		for (std::size_t i = 0; i < sizeof(T); ++i)
			buffer[i] = static_cast<std::byte>(bytes[i]);

		return std::bit_cast<T>(buffer);
	}

	template<typename T>
	constexpr std::optional<T> get(Keys key) requires(std::is_trivially_copyable_v<T>)
	{
		auto parsing = blob;

		while (parsing.size() > sizeof(Header)) {
			auto header = as<Header>(parsing);

			parsing = parsing.subspan(sizeof(Header));

			if (header.key == key) {
				if (header.size == sizeof(T)) {
					return as<T>(parsing);
				} else {
					// TODO: report incorrect size error?
					// ...or ignore and continue trying to find the key
				}
			}
			parsing = parsing.subspan(header.size);
		}
		return std::nullopt;
	}
};

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
