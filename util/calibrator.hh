#pragma once
#include <cmath>

class Calibrator {
public:
	Calibrator() = default;
	constexpr Calibrator(float slope, float offset)
		: _slope(slope)
		, _offset(offset) {
	}

	constexpr Calibrator(std::pair<float, float> target, std::pair<float, float> measurement) {
		calibrate_chan(target, measurement);
	}

	constexpr float adjust(float raw) {
		return (raw - _offset) * _slope;
	}

	void set_slope_offset(float slope, float offset) {
		_slope = slope;
		_offset = offset;
	}

	constexpr void calibrate_chan(float target_low, float target_high, float low_measurement, float high_measurement) {
		_slope = (target_high - target_low) / (high_measurement - low_measurement);
		_offset = low_measurement - (target_low / _slope);
	}

	constexpr void calibrate_chan(std::pair<float, float> target, std::pair<float, float> measurement) {
		calibrate_chan(target.first, target.second, measurement.first, measurement.second);
	}

	float slope() const {
		return _slope;
	}

	float offset() const {
		return _offset;
	}

private:
	float _slope = 1.0f;
	float _offset = 0.f;
};
