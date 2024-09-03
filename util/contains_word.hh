#include <algorithm>
#include <string>

inline bool contains_word(std::string const &name, std::string const &word) {
	// Case-insensitive:
	std::string lname = name;
	std::transform(lname.begin(), lname.end(), lname.begin(), [](char c) { return std::tolower(c); });
	std::string lword = word;
	std::transform(lword.begin(), lword.end(), lword.begin(), [](char c) { return std::tolower(c); });

	std::string space_lword = " " + lword;
	std::string lword_space = lword + " ";
	std::string space_lword_space = " " + lword + " ";

	// "in"
	if (lname == lword)
		return true;

	// "play in", but not "plain"
	if (lname.ends_with(space_lword))
		return true;

	// "in 1" but not "inner gain"
	if (lname.starts_with(lword_space))
		return true;

	// "chan in 1", but not "chain 1" or "ch1 into ch2"
	if (lname.find(space_lword_space) != std::string::npos)
		return true;

	return false;
}
