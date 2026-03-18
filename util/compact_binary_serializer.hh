#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

// Serializes data into the compact binary format: [key][data-size][data]
// Compatible with CompactBinaryParser.
//
// Sub-objects are supported via nested callables. Each sub-object callable
// is invoked twice internally: once to measure its size, once to write.
// This avoids needing to buffer sub-object data in memory.
//
// Example:
//   auto describe = [](auto& s) {
//       s.field(Keys::BPM, uint32_t(120));
//       s.object(Keys::Track, [](auto& s) {
//           s.field(Keys::Length, uint16_t(8));
//       });
//   };
//   std::array<uint8_t, 64> buf;
//   auto n = CompactBinarySerializer<Keys>::serialize(buf, describe);
//
// Assumptions:
// Little-endian

template<typename KeyT, typename DataSizeT = uint16_t>
requires(std::is_trivial_v<KeyT> && std::is_integral_v<DataSizeT>)
struct CompactBinarySerializer {

	struct Measurer {
		size_t count = 0;

		template<typename T>
		constexpr void field(KeyT, T const &) requires(std::is_trivially_copyable_v<T>)
		{
			count += sizeof(KeyT) + sizeof(DataSizeT) + sizeof(T);
		}

		constexpr void raw(KeyT, std::span<const uint8_t> data) {
			count += sizeof(KeyT) + sizeof(DataSizeT) + data.size();
		}

		template<typename F>
		constexpr void object(KeyT, F &&sub) {
			Measurer sub_m;
			sub(sub_m);
			count += sizeof(KeyT) + sizeof(DataSizeT) + sub_m.count;
		}
	};

	template<typename Derived>
	struct WriterBase {
		constexpr Derived &self() {
			return static_cast<Derived &>(*this);
		}

		constexpr void write_key(KeyT key) {
			if constexpr (std::is_array_v<KeyT>) {
				std::array<uint8_t, sizeof(KeyT)> buf{};
				for (size_t i = 0; i < sizeof(KeyT); ++i) {
					auto c = key[i];
					buf[i] = static_cast<uint8_t>(c);
					if (c == '\0')
						break;
				}
				self().emit(std::span<const uint8_t>{buf});
			} else {
				auto bytes = std::bit_cast<std::array<uint8_t, sizeof(KeyT)>>(key);
				self().emit(std::span<const uint8_t>{bytes});
			}
		}

		constexpr void write_size(DataSizeT size) {
			auto bytes = std::bit_cast<std::array<uint8_t, sizeof(DataSizeT)>>(size);
			self().emit(std::span<const uint8_t>{bytes});
		}

		template<typename T>
		constexpr void field(KeyT key, T const &value) requires(std::is_trivially_copyable_v<T>)
		{
			write_key(key);
			write_size(static_cast<DataSizeT>(sizeof(T)));
			auto bytes = std::bit_cast<std::array<uint8_t, sizeof(T)>>(value);
			self().emit(std::span<const uint8_t>{bytes});
		}

		constexpr void raw(KeyT key, std::span<const uint8_t> data) {
			write_key(key);
			write_size(static_cast<DataSizeT>(data.size()));
			self().emit(data);
		}

		template<typename F>
		constexpr void object(KeyT key, F &&sub) {
			Measurer m;
			sub(m);
			write_key(key);
			write_size(static_cast<DataSizeT>(m.count));
			sub(self());
		}
	};

	struct SpanWriter : WriterBase<SpanWriter> {
		std::span<uint8_t> buf;
		size_t pos = 0;

		constexpr SpanWriter(std::span<uint8_t> buf)
			: buf{buf} {
		}

		constexpr void emit(std::span<const uint8_t> bytes) {
			for (auto b : bytes) {
				if (pos < buf.size())
					buf[pos] = b;
				pos++;
			}
		}
	};

	template<typename Sink>
	struct SinkWriter : WriterBase<SinkWriter<Sink>> {
		Sink &sink;
		size_t count = 0;

		constexpr SinkWriter(Sink &sink)
			: sink{sink} {
		}

		constexpr void emit(std::span<const uint8_t> bytes) {
			sink(bytes);
			count += bytes.size();
		}
	};

	template<typename F>
	constexpr static size_t measure(F &&describe) {
		Measurer m;
		describe(m);
		return m.count;
	}

	// Returns total bytes needed (may exceed output.size() on overflow)
	template<typename F>
	constexpr static size_t serialize(std::span<uint8_t> output, F &&describe) {
		SpanWriter w{output};
		describe(w);
		return w.pos;
	}

	template<typename F, typename Sink>
	constexpr static size_t stream(F &&describe, Sink &&sink) {
		SinkWriter<Sink> w{sink};
		describe(w);
		return w.count;
	}
};
