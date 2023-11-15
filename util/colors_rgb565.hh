#pragma once
#include <cstdint>

struct Color565 {
	uint16_t r : 5;
	uint16_t g : 6;
	uint16_t b : 5;

	constexpr Color565(uint8_t red, uint8_t green, uint8_t blue) {
		r = red >> 3;
		g = green >> 2;
		b = blue >> 3;
	}
	constexpr uint16_t raw() const {
		return (r << 11) | (g << 5) | b;
	}
	//0..255
	constexpr uint8_t red() const {
		return (raw() & 0xf800) >> 8;
	}
	//0..255
	constexpr uint8_t green() {
		return (raw() & 0x07e0) >> 3;
	}
	//0..255
	constexpr uint8_t blue() {
		return (raw() & 0x001f) << 3;
	}
};
namespace Colors565
{
static constexpr auto Red = Color565{0xFF, 0x00, 0x00};
static constexpr auto Orange = Color565{0xFF, 0xAA, 0x00};
static constexpr auto Yellow = Color565{0xFF, 0xFF, 0x00};
static constexpr auto Green = Color565{0x00, 0xFF, 0x00};
static constexpr auto Blue = Color565{0x00, 0x00, 0xFF};
static constexpr auto Cyan = Color565{0x00, 0xFF, 0xFF};
static constexpr auto Purple = Color565{0x88, 0x00, 0xFF};
static constexpr auto Pink = Color565{0xFF, 0x00, 0xAA};
static constexpr auto White = Color565{0xFF, 0xFF, 0xFF};
static constexpr auto Black = Color565{0x00, 0x00, 0x00};
static constexpr auto Grey = Color565{0x80, 0x80, 0x80};
} // namespace Colors565

static_assert(sizeof(Color565) == 2);
static_assert(Color565{0xFF, 0x00, 0x00}.red() == (0xFF & 0xF8));
static_assert(Color565{0xAA, 0xFF, 0xFF}.red() == (0xAA & 0xF8));
static_assert(Color565{0x55, 0xFF, 0xFF}.red() == (0x55 & 0xF8));
static_assert(Color565{0x00, 0xFF, 0x00}.green() == (0xFF & 0xFC));
static_assert(Color565{0xFF, 0xAA, 0xFF}.green() == (0xAA & 0xFC));
static_assert(Color565{0xFF, 0x55, 0xFF}.green() == (0x55 & 0xFC));
static_assert(Color565{0x00, 0x00, 0xFF}.blue() == (0xFF & 0xF8));
static_assert(Color565{0xFF, 0xFF, 0xAA}.blue() == (0xAA & 0xF8));
static_assert(Color565{0xFF, 0xFF, 0x55}.blue() == (0x55 & 0xF8));
static_assert(Color565{0xFF, 0xFF, 0xFF}.raw() == 0xFFFF);
static_assert(Color565{0x80, 0x80, 0x80}.raw() == 0x8410);
static_assert(Color565{0x00, 0x00, 0x00}.raw() == 0x0000);
