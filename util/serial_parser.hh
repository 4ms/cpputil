#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

template<typename Keys, typename DataSizeT = uint16_t>
struct Parser {
	struct __attribute__((packed)) Header {
		Keys key;
		DataSizeT size;
	};

	std::span<const uint8_t> blob;

	constexpr Parser(std::span<const uint8_t> blob)
		: blob{blob} {
	}

	template<typename T>
	constexpr static T as(std::span<const uint8_t> bytes) {
		// Copy to ensure alignment:
		alignas(T) std::array<std::byte, sizeof(T)> buffer;

		for (std::size_t i = 0; i < sizeof(T); ++i)
			buffer[i] = static_cast<std::byte>(bytes[i]);

		return std::bit_cast<T>(buffer);
	}

	constexpr bool advance(std::span<const uint8_t> &block, size_t offset) {
		if (offset >= block.size())
			return false;

		block = block.subspan(offset);
		return true;
	}

	template<typename T>
	constexpr std::optional<T> get(Keys key) requires(std::is_trivially_copyable_v<T>)
	{
		auto parsing = blob;

		while (parsing.size() >= sizeof(Header)) {
			auto header = as<Header>(parsing);

			if (!advance(parsing, sizeof(Header)))
				break;

			bool cmp;
			if constexpr (std::is_array_v<Keys>)
				cmp = (c_memcmp(header.key, key, sizeof(Keys)) == 0);
			else
				cmp = (header.key == key);

			if (cmp) {
				if (header.size == sizeof(T)) {
					if (parsing.size() >= sizeof(T))
						return as<T>(parsing);
				}
			}

			if (!advance(parsing, header.size))
				break;
		}
		return std::nullopt;
	}

	// constexpr memcmp
	constexpr static int c_memcmp(const char *a, const char *b, size_t s) {
		while (s--) {
			if (*a++ != *b++)
				return (*a < *b) ? -1 : 1;
		}
		return 0;
	}
};
