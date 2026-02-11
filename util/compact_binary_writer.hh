#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

// Imperative writer for the compact binary format: [key][data-size][data]
// Compatible with CompactBinaryParser.
//
// Writes sequentially into a user-provided buffer. Sub-objects use
// begin_object()/end_object() pairs which backpatch the size field.
//
// Example:
//   std::array<uint8_t, 64> buf;
//   CompactBinaryWriter<Keys> w{buf};
//   w.field(Keys::BPM, uint32_t(120));
//   w.begin_object(Keys::Track);
//       w.field(Keys::Length, uint16_t(8));
//   w.end_object();
//   auto n = w.size(); // total bytes written
//
// size() may exceed buf.size() on overflow; no out-of-bounds writes occur.
//
// Assumptions:
// Little-endian

template<typename KeyT, typename DataSizeT = uint16_t, unsigned MaxDepth = 8>
requires(std::is_trivial_v<KeyT> && std::is_integral_v<DataSizeT>)
struct CompactBinaryWriter {

	std::span<uint8_t> buf;
	size_t pos = 0;

	struct Mark {
		size_t size_pos;
	};
	std::array<Mark, MaxDepth> stack{};
	unsigned depth = 0;

	constexpr CompactBinaryWriter(std::span<uint8_t> output)
		: buf{output} {
	}

	template<typename T>
	constexpr void field(KeyT key, T const &value) requires(std::is_trivially_copyable_v<T>) {
		write_key(key);
		write_size(static_cast<DataSizeT>(sizeof(T)));
		auto bytes = to_bytes(value);
		emit(std::span<const uint8_t>{bytes});
	}

	constexpr void raw(KeyT key, std::span<const uint8_t> data) {
		write_key(key);
		write_size(static_cast<DataSizeT>(data.size()));
		emit(data);
	}

	constexpr void begin_object(KeyT key) {
		write_key(key);
		if (depth < MaxDepth)
			stack[depth] = Mark{pos};
		depth++;
		write_size(DataSizeT{0});
	}

	constexpr void end_object() {
		if (depth == 0)
			return;
		depth--;
		if (depth < MaxDepth) {
			auto data_start = stack[depth].size_pos + sizeof(DataSizeT);
			auto data_size = static_cast<DataSizeT>(pos - data_start);
			auto bytes = to_bytes(data_size);
			write_at(stack[depth].size_pos, std::span<const uint8_t>{bytes});
		}
	}

	constexpr size_t size() const { return pos; }

private:
	template<typename T>
	constexpr static auto to_bytes(T const &value) -> std::array<uint8_t, sizeof(T)>
	requires(std::is_trivially_copyable_v<T>)
	{
		return std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
	}

	constexpr void emit(std::span<const uint8_t> bytes) {
		for (auto b : bytes) {
			if (pos < buf.size())
				buf[pos] = b;
			pos++;
		}
	}

	constexpr void write_at(size_t position, std::span<const uint8_t> bytes) {
		for (size_t i = 0; i < bytes.size(); ++i) {
			if (position + i < buf.size())
				buf[position + i] = bytes[i];
		}
	}

	constexpr void write_key(KeyT key) {
		if constexpr (std::is_array_v<KeyT>) {
			std::array<uint8_t, sizeof(KeyT)> tmp{};
			for (size_t i = 0; i < sizeof(KeyT); ++i) {
				auto c = key[i];
				tmp[i] = static_cast<uint8_t>(c);
				if (c == '\0')
					break;
			}
			emit(std::span<const uint8_t>{tmp});
		} else {
			auto bytes = to_bytes(key);
			emit(std::span<const uint8_t>{bytes});
		}
	}

	constexpr void write_size(DataSizeT sz) {
		auto bytes = to_bytes(sz);
		emit(std::span<const uint8_t>{bytes});
	}
};
