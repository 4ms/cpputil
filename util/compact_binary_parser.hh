#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>

// Parses binary data compacted with [key][data-size][data]
// [key] is a fixed width, defined by KeyT
// [data-size] is a fixed width, defined by DataSizeT
// [data] is #data-size bytes long
//
// Example with KeyT == DataSizeT == uint16_t
//
//  0x05 0x00  0x04 0x00  0xff 0x11 0x33 0xaa
// ----------  ---------  -------------------
//  key = 5    d-sz=4Bytes  data = 0xaa3311ff
//
// Can also use char[] for keys (e.g. "bpm"):
// KeyT == const char[6], DataSizeT == uint8_t
//
//  'b' 'p' 'm'   0x01     0x80
// ----------     ----     ----
//  key="bpm"    d-sz=1B   data=128
//
// If the key is smaller than the max key size then it must be padded.
// The first padding byte must be \0 (null-terminator).
// Subsequent padding bytes can be any value.
//
//        v----v---required padding since all keys must be same length
//  'x' '\0'  0xXX   0x01     0x05
// ---------------   ----     ----
//  key="x"         d-sz=1B   data=5
//       ^---lookup key is 'x' '\0' which matches 'x' '\0' 0xXX
//
// Assumptions:
// Little-endian
// Lifetime of backing data exceeds that of CompactBinaryParser object

template<typename KeyT, typename DataSizeT = uint16_t>
requires(std::is_trivial_v<KeyT> && std::is_integral_v<DataSizeT>)
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
			// stop comparing if both arrays have null terminator in the same place
			if (*a == 0 && *b == 0)
				return 0;
			if (*a++ != *b++)
				return -1;
		}
		return 0;
	}
};
