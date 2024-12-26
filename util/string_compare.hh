#pragma once
#include <algorithm>
#include <cctype>
#include <string_view>

//case insensitive less
constexpr bool less_ci(std::string_view a, std::string_view b) {
	return std::ranges::lexicographical_compare(
		a, b, [](const char char1, const char char2) { return std::tolower(char1) < std::tolower(char2); });
}
