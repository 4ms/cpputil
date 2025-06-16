#pragma once
#include <algorithm>
#include <cctype>
#include <string_view>

// case insensitive less
constexpr bool less_ci(std::string_view a, std::string_view b) {
	return std::ranges::lexicographical_compare(a, b, [](const char char1, const char char2) {
		return std::tolower(static_cast<unsigned char>(char1)) < std::tolower(static_cast<unsigned char>(char2));
	});
}

// case insensitive ==
constexpr bool equal_ci(std::string_view a, std::string_view b) {
	return std::ranges::equal(a, b, [](char char1, char char2) {
		return std::tolower(static_cast<unsigned char>(char1)) == std::tolower(static_cast<unsigned char>(char2));
	});
}
