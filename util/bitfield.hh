#pragma once
#include <cstdint>

template<typename T>
uint8_t constexpr bitfield(T const val) {
	return (static_cast<uint8_t>(val));
}
template<typename... Args>
uint8_t constexpr bitfield(const Args... args) {
	return (... | bitfield(args));
}
