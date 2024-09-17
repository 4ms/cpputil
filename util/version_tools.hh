#pragma once
#include <ranges>
#include <string_view>

namespace VersionUtil
{

struct Version {
	unsigned major;
	unsigned minor;
	unsigned revision;

	void set_field(unsigned index, unsigned val) {
		if (index == 0)
			major = val;
		if (index == 1)
			minor = val;
		if (index == 2)
			revision = val;
	}
};

auto split(std::string_view str, std::string_view delim) {
	auto tokens = str | std::ranges::views::split(delim) | std::ranges::views::transform([](auto &&str) {
		return std::string_view(&*str.begin(), std::ranges::distance(str));
	});
	return tokens;
}

Version parse_version(std::string_view vers) {
	auto tokens = split(vers, ".");
	Version version{};

	for (unsigned i = 0; auto &&token : tokens) {
		auto num = strtoul(token.data(), nullptr, 10);

		version.set_field(i, num);

		if (++i >= 3)
			break;
	}
	return version;
}

} // namespace VersionUtil
