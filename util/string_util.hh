#pragma once
#include <string>
#include <vector>

struct ReplaceString {
	std::string str;

	ReplaceString(std::string initval)
		: str{initval} {
	}

	ReplaceString &replace_all(const std::string str_find, const std::string str_replace) {
		size_t index = 0;
		while (true) {
			index = str.find(str_find, index);
			if (index == std::string::npos) {
				break;
			}
			str.replace(index, str_find.length(), str_replace);
			index += str_find.length();
		}
		return *this;
	}
};

inline void
tokenize_string(std::string_view str, std::vector<std::string> &tokens, std::string const &delimiters = ",") {
	size_t lastPos = str.find_first_not_of(delimiters, 0);

	// Find first non-delimiter.
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (pos != std::string::npos || lastPos != std::string::npos) {
		// Found a token, add it to the vector.
		tokens.push_back(std::string(str.substr(lastPos, pos - lastPos)));

		// Skip delimiters.
		lastPos = str.find_first_not_of(delimiters, pos);

		// Find next non-delimiter.
		pos = str.find_first_of(delimiters, lastPos);
	}
}
