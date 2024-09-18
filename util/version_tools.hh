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
		//sanity check:
		if (val > 9999)
			return;

		if (index == 0)
			major = val;
		if (index == 1)
			minor = val;
		if (index == 2)
			revision = val;
	}
};

#if !defined(__cpp_lib_ranges) || (__cpp_lib_ranges < 202211L)

Version parse_version(std::string_view vers) {
	Version version{};
	const std::string_view delim = ".";

	unsigned i = 0;
	while (true) {
		auto token = vers;
		const auto pos = vers.find_first_of(delim);
		if (pos != std::string_view::npos) {
			token = vers.substr(0, pos);
			vers = vers.substr(pos + 1);
		}

		auto num = strtol(token.data(), nullptr, 10);
		version.set_field(i++, num);

		if (pos == std::string_view::npos)
			break;
	}
	return version;
}

#else

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
#endif

} // namespace VersionUtil
