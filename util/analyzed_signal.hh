#pragma once
#include <cstdint>
#include <limits>

struct AnalyzedSig {
	AnalyzedSig(float iircoef)
		: iir_coef{iircoef} {
	}

	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();
	float avg = 0.f;
	float iir = 0.f;

	void update(float newval) {
		if (newval < min)
			min = newval;
		if (newval > max)
			max = newval;
		iir += iir_coef * (newval - iir);
		avg = iir; //TODO
	}

	void reset_to(float curval) {
		avg = curval;
		iir = curval;
		min = curval; // std::numeric_limits<float>::max();
		max = curval; // std::numeric_limits<float>::min();
	}

private:
	float iir_coef;
};

template<uint32_t iir_coef_int>
struct AnalyzedSignal : AnalyzedSig {
	AnalyzedSignal()
		: AnalyzedSig{1.f / (float)iir_coef_int} {
	}
};

template<typename T>
struct PeakMeter {
	// TODO: attack, decay

	T min = std::numeric_limits<T>::max();
	T max = std::numeric_limits<T>::min();

	void update(T newval) {
		if (newval < min)
			min = newval;
		if (newval > max)
			max = newval;
	}

	void reset() {
		min = std::numeric_limits<T>::max();
		max = std::numeric_limits<T>::min();
	}
};
