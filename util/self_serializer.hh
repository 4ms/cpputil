#pragma once

#include <cstdint>
#include <span>
#include <type_traits>

template<typename T>
concept Serializable = requires {
	//requires std::is_aggregate_v<T>;
	requires !std::is_polymorphic_v<T>;
};

struct Serializer {
	template<Serializable Self>
	constexpr auto serialize(this const Self &self) {
		return std::span{reinterpret_cast<const uint8_t *>(&self), sizeof(Self)};
	}
};
