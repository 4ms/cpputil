#pragma once

#include <cstdint>
#include <limits>
#include <type_traits>

template<uint64_t max_val>
struct SmallestUnsignedInt {
	using type = std::conditional_t<
		max_val <= std::numeric_limits<uint8_t>::max(),
		uint8_t,
		std::conditional_t<max_val <= std::numeric_limits<uint16_t>::max(),
						   uint16_t,
						   std::conditional_t<max_val <= std::numeric_limits<uint32_t>::max(), uint32_t, uint64_t>>>;
};

template<int64_t min_val, int64_t max_val>
struct SmallestSignedInt {
	using type =
		std::make_signed_t<typename SmallestUnsignedInt<(max_val >= -min_val ? max_val : -min_val - 1) << 1>::type>;
};

static_assert(std::is_same_v<SmallestUnsignedInt<0>::type, uint8_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<255>::type, uint8_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<256>::type, uint16_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<65535>::type, uint16_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<65536>::type, uint32_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<(1ull << 32) - 1>::type, uint32_t>);
static_assert(std::is_same_v<SmallestUnsignedInt<1ull << 32>::type, uint64_t>);

static_assert(std::is_same_v<SmallestSignedInt<0, 127>::type, int8_t>);
static_assert(std::is_same_v<SmallestSignedInt<-128, 127>::type, int8_t>);
static_assert(std::is_same_v<SmallestSignedInt<0, 128>::type, int16_t>);
static_assert(std::is_same_v<SmallestSignedInt<-129, 0>::type, int16_t>);
static_assert(std::is_same_v<SmallestSignedInt<0, 32767>::type, int16_t>);
static_assert(std::is_same_v<SmallestSignedInt<-32768, 32767>::type, int16_t>);
static_assert(std::is_same_v<SmallestSignedInt<-32768, 32768>::type, int32_t>);
static_assert(std::is_same_v<SmallestSignedInt<-3293768, 3932768>::type, int32_t>);
