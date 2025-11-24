#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>

struct RGB565 {
	// msb> rrrrr gggggg bbbbb <lsb
	uint16_t b : 5;
	uint16_t g : 6;
	uint16_t r : 5;

	constexpr RGB565() {
	}

	constexpr RGB565(uint8_t red, uint8_t green, uint8_t blue)
		: b(blue >> 3)
		, g(green >> 2)
		, r(red >> 3) {
	}

	constexpr RGB565(float red, float green, float blue)
		: b(std::clamp<uint16_t>(blue * 32.f, 0, 31))
		, g(std::clamp<uint16_t>(green * 64.f, 0, 63))
		, r(std::clamp<uint16_t>(red * 32.f, 0, 31)) {
	}

	constexpr RGB565(uint32_t rgb888)
		: b((rgb888 & 0x0000f8) >> 3)
		, g((rgb888 & 0x00fc00) >> 10)
		, r((rgb888 & 0xf80000) >> 19) {
	}

	// Construct from raw u16 RGB565 format
	RGB565 operator=(uint16_t raw) {
		*this = std::bit_cast<RGB565>(raw);
		return *this;
	}

	constexpr uint16_t raw() const {
		return (r << 11) | (g << 5) | b;
	}

	constexpr operator uint16_t() const {
		return std::bit_cast<uint16_t>(*this);
	}

	//0..255
	constexpr uint8_t red() const {
		return r << 3;
	}

	//0..255
	constexpr uint8_t green() const {
		return g << 2;
	}

	//0..255
	constexpr uint8_t blue() const {
		return b << 3;
	}
};
namespace Colors565
{
static constexpr auto Red = RGB565{(uint8_t)0xFF, 0x00, 0x00};
static constexpr auto Orange = RGB565{(uint8_t)0xFF, 0xAA, 0x00};
static constexpr auto Yellow = RGB565{(uint8_t)0xFF, 0xFF, 0x00};
static constexpr auto Green = RGB565{(uint8_t)0x00, 0xFF, 0x00};
static constexpr auto Blue = RGB565{(uint8_t)0x00, 0x00, 0xFF};
static constexpr auto Cyan = RGB565{(uint8_t)0x00, 0xFF, 0xFF};
static constexpr auto Purple = RGB565{(uint8_t)0x88, 0x00, 0xFF};
static constexpr auto Pink = RGB565{(uint8_t)0xFF, 0x00, 0xAA};
static constexpr auto White = RGB565{(uint8_t)0xFF, 0xFF, 0xFF};
static constexpr auto Black = RGB565{(uint8_t)0x00, 0x00, 0x00};
static constexpr auto Grey = RGB565{(uint8_t)0x80, 0x80, 0x80};
} // namespace Colors565

static_assert(sizeof(RGB565) == 2);
static_assert(RGB565{(uint8_t)0xFF, 0x00, 0x00}.red() == (0xFF & 0xF8));
static_assert(RGB565{(uint8_t)0xAA, 0xFF, 0xFF}.red() == (0xAA & 0xF8));
static_assert(RGB565{(uint8_t)0x55, 0xFF, 0xFF}.red() == (0x55 & 0xF8));
static_assert(RGB565{(uint8_t)0x00, 0xFF, 0x00}.green() == (0xFF & 0xFC));
static_assert(RGB565{(uint8_t)0xFF, 0xAA, 0xFF}.green() == (0xAA & 0xFC));
static_assert(RGB565{(uint8_t)0xFF, 0x55, 0xFF}.green() == (0x55 & 0xFC));
static_assert(RGB565{(uint8_t)0x00, 0x00, 0xFF}.blue() == (0xFF & 0xF8));
static_assert(RGB565{(uint8_t)0xFF, 0xFF, 0xAA}.blue() == (0xAA & 0xF8));
static_assert(RGB565{(uint8_t)0xFF, 0xFF, 0x55}.blue() == (0x55 & 0xF8));
static_assert(RGB565{(uint8_t)0xFF, 0xFF, 0xFF}.raw() == 0xFFFF);
static_assert(RGB565{(uint8_t)0x80, 0x80, 0x80}.raw() == 0x8410);
static_assert(RGB565{(uint8_t)0x00, 0x00, 0x00}.raw() == 0x0000);

static_assert(RGB565(0xFFAA22).raw() == RGB565((uint8_t)0xFF, 0xAA, 0x22).raw());
