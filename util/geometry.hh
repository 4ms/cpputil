#pragma once
#include <cstdint>
#include <span>

struct Rect;

struct RectC {
	int32_t left;
	int32_t top;
	int32_t right;
	int32_t bottom;

	constexpr int32_t width() const {
		return right - left;
	}
	constexpr int32_t height() const {
		return bottom - top;
	}
	constexpr int32_t center_y() const {
		return (top + bottom) / 2;
	}
	constexpr int32_t center_x() const {
		return (left + right) / 2;
	}

	// operator Rect();

	constexpr bool y_intersects(const RectC &other) const {
		return (top >= other.top && top <= other.bottom) || (other.top >= top && other.top <= bottom);
	}
	constexpr bool x_intersects(const RectC &other) const {
		return (left >= other.left && left <= other.right) || (right <= other.right && right >= other.left);
	}
	constexpr bool is_intersecting(const RectC &other) const {
		return y_intersects(other) && x_intersects(other);
	}
};

struct Rect {
	int32_t left;
	int32_t top;
	int32_t width;
	int32_t height;
	constexpr int32_t right() const {
		return left + width;
	}
	constexpr int32_t bottom() const {
		return top + height;
	}
	constexpr int32_t center_y() const {
		return top + height / 2;
	}
	constexpr int32_t center_x() const {
		return left + width / 2;
	}
	// operator RectC();
};

// RectC::operator Rect()
// {
// 	return Rect{.left = left, .top = top, .width = width(), .height = height()};
// }
// Rect::operator RectC()
// {
// 	return RectC{.left = left, .top = top, .right = right(), .bottom = bottom()};
// }

template<typename T>
constexpr bool is_poly_concave(std::span<const T> points) requires requires(T pt) {
	pt.x;
	pt.y;
}
{
	auto n = points.size();
	if (n < 3)
		return false;

	auto X_prod = [](T O, T A, T B) { return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x); };

	int sign = 0;
	for (auto i = 0u; i < points.size(); i++) {
		auto O = points[i];
		auto A = points[(i + 1) % n];
		auto B = points[(i + 2) % n];
		auto cross = X_prod(O, A, B);
		if (cross == 0)
			continue;

		if (sign == 0)
			sign = cross;

		if ((sign > 0 && cross < 0) || (sign < 0 && cross > 0))
			return true;
	}
	return false;
}

namespace CpputilGeometry
{

struct TestPoint {
	uint8_t x;
	uint8_t y;
};

constexpr std::array<TestPoint, 4> tri{{{1, 1}, {2, 1}, {2, 2}, {1, 2}}};
static_assert(is_poly_concave<TestPoint>(tri) == false);

constexpr std::array<TestPoint, 5> cave{{{1, 1}, {2, 1}, {2, 0}, {2, 2}, {1, 2}}};
static_assert(is_poly_concave<TestPoint>(cave) == true);

} // namespace CpputilGeometry
