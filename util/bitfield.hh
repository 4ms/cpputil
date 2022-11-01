#pragma once
#include <cstdint>
#include <type_traits>

template<typename T>
auto constexpr bitfield(T const val) {
	return static_cast<std::underlying_type_t<T>>(val);
}
template<typename... Args>
auto constexpr bitfield(const Args... args) {
	return (... | bitfield(args));
}
