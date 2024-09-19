#pragma once
#include <cstdlib>
#include <cstdint>
#include <string_view>

namespace VersionUtil
{

struct Version {
	uint8_t major{};
	uint8_t minor{};
	uint8_t revision{};

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

	Version() = default;

	Version(uint8_t maj, uint8_t min, uint8_t rev)
		: major{maj}
		, minor{min}
		, revision{rev} {
	}

	Version(std::string_view vers) {
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
			set_field(i++, num);

			if (pos == std::string_view::npos)
				break;
		}
	}

	// A Host is compatible with a plugin that has the same major and less-than-or-equal minor.
	bool can_host_version(Version other) const {
		return major == other.major && minor >= other.minor;
	}

	auto operator<=>(Version const &other) const {
		if (major == other.major && minor == other.minor && revision == other.revision)
			return std::strong_ordering::equal;

		if ((major < other.major) || (major == other.major && minor < other.minor) ||
			(major == other.major && minor == other.minor && revision < other.revision))
			return std::strong_ordering::less;

		return std::strong_ordering::greater;
	}

	bool operator==(Version const &other) const = default;

	bool is_later(Version const &other) const {
		return *this > other;
	}
};

} // namespace VersionUtil
