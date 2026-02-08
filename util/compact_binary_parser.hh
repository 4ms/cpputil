#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

// Parses binary data compacted with [key][data-size][data]
//
//  0x05 0x00  0x04 0x00  0xff 0x3f 0x33 0x00
// ----------  ---------  -------------------
//  key = 5    data=4Bytes  data = 0x00333fff
//
// Can also use char[] for keys (e.g. "bpm")
//
// Assumptions:
// Little-endian
// Lifetime of backing data exceeds that of CompactBinaryParser object

template<typename KeyT, typename DataSizeT = uint16_t>
struct CompactBinaryParser {
	struct __attribute__((packed)) Header {
		KeyT key;
		DataSizeT size;
	};

	std::span<const uint8_t> blob;

	constexpr CompactBinaryParser(std::span<const uint8_t> blob)
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

	constexpr static bool advance(std::span<const uint8_t> &block, size_t offset) {
		if (offset >= block.size())
			return false;

		block = block.subspan(offset);
		return true;
	}

	template<typename T>
	constexpr std::optional<T> get(KeyT key) const requires(std::is_trivially_copyable_v<T>)
	{
		auto parsing = blob;

		while (parsing.size() >= sizeof(Header)) {
			auto header = as<Header>(parsing);

			if (!advance(parsing, sizeof(Header)))
				break;

			bool cmp;
			if constexpr (std::is_array_v<KeyT>)
				cmp = (c_memcmp(header.key, key, sizeof(KeyT)) == 0);
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

	// constexpr memcmp (almost)
	constexpr static int c_memcmp(const char *a, const char *b, size_t s) {
		while (s--) {
			if (*a++ != *b++)
				return -1;
		}
		return 0;
	}
};
