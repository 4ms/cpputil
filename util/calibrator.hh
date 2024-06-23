#pragma once
#include <utility>

// Linear transformation used for calibrating ADCs and DACs
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

	constexpr float adjust(float raw) const {
		return (raw - _offset) * _slope;
	}

	constexpr float reverse_calibrate(float val) const {
		return val / _slope + _offset;
	}

	void set_slope_offset(float slope, float offset) {
		_slope = slope;
		_offset = offset;
	}

	constexpr void calibrate_chan(float target_low, float target_high, float low_measurement, float high_measurement) {
		_slope = (target_high - target_low) / (high_measurement - low_measurement);
		_offset = low_measurement - (target_low / _slope);
	}

	// calibrate_chan(TARGET, MEASUREMENT)
	// outputs cal: when the MCU writes TARGET value to the codec DAC, the user sees MEASUREMENT volts on the jack.
	// inputs cal: when the user inputs TARGET volts to the jack, the codec reads MEASUREMENT value.
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

static_assert(Calibrator{1.05f, -.003f}.reverse_calibrate(Calibrator{1.05f, -.003f}.adjust(9.32f)) == 9.32f);
static_assert(Calibrator{1.05f, -.003f}.adjust(Calibrator{1.05f, -.003f}.reverse_calibrate(9.32f)) == 9.32f);
