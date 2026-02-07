#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <span>

template<typename Keys, typename DataSizeT = uint16_t>
struct Parser {
	struct Header {
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
		alignas(T) std::array<std::byte, sizeof(T)> buffer{};

		for (std::size_t i = 0; i < sizeof(T); ++i)
			buffer[i] = static_cast<std::byte>(bytes[i]);

		return std::bit_cast<T>(buffer);
	}

	template<typename T>
	constexpr std::optional<T> get(Keys key) requires(std::is_trivially_copyable_v<T>)
	{
		auto parsing = blob;

		while (parsing.size() > sizeof(Header)) {
			auto header = as<Header>(parsing);

			parsing = parsing.subspan(sizeof(Header));

			if (header.key == key) {
				if (header.size == sizeof(T)) {
					return as<T>(parsing);
				} else {
					// TODO: report incorrect size error?
					// ...or ignore and continue trying to find the key
				}
			}
			parsing = parsing.subspan(header.size);
		}
		return std::nullopt;
	}
};
