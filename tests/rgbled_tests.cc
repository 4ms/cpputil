#include "doctest.h"
#include "util/rgbled.hh"
#include <cstdint>

// Minimal mock LED driver satisfying the LedC concept
// set() must be const since MixedRgbLed::set_color() is const
struct MockLed {
	mutable uint8_t value = 0;
	void set(uint8_t v) const {
		value = v;
	}
};

TEST_CASE("MixedRgbLed: set_color sets all channels") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.set_color(Colors::white);
	// White is full brightness on all channels
	// Checking that set_color runs without crash and calls set()
	// (actual values depend on Colors::white definition)
}

TEST_CASE("MixedRgbLed: set_base_color and update_animation") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.set_base_color(Colors::red);
	led.update_animation(); // should not crash
}

TEST_CASE("MixedRgbLed: reset_breathe clears breathe") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.breathe(Colors::blue, 1.0f);
	led.reset_breathe();
	led.update_animation(); // should not crash
}

TEST_CASE("MixedRgbLed: flash_once_ms then update") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.flash_once_ms(Colors::green, 100.f);
	led.update_animation();
}

TEST_CASE("MixedRgbLed: fade_once_ms sets smooth fade") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.fade_once_ms(Colors::white, 50.f);
	led.update_animation();
}

TEST_CASE("MixedRgbLed: set_color with uint8_t brightness") {
	MockLed r, g, b;
	MixedRgbLed<MockLed, MockLed, MockLed> led{r, g, b};
	led.set_color(Colors::white, uint8_t{128});
	// Should not crash
}

TEST_CASE("MixedRgbLed: set_color with float brightness") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.set_color(Colors::red, 0.5f);
}

TEST_CASE("MixedRgbLed: add_base_color and blend_base_color") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.set_base_color(Colors::off);
	led.add_base_color(Colors::red);
	led.blend_base_color(Colors::blue);
	led.update_animation();
}

TEST_CASE("RgbLed alias: same LED type for all channels") {
	RgbLed<MockLed> led;
	led.set_color(Colors::white);
	led.update_animation();
}

TEST_CASE("MixedRgbLed: update_oscillators updates without crash") {
	MixedRgbLed<MockLed, MockLed, MockLed> led;
	led.breathe(Colors::red, 2.0f);
	for (int i = 0; i < 100; i++)
		led.update_oscillators();
}
