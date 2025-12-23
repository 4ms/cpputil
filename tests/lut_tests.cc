#include "doctest.h"
#include "util/lut.hh"

struct VoltageToFreqTableRange {
	static constexpr float min = -0.1f;
	static constexpr float max = 0.5f;
};
constinit auto VoltageToFrequencyTable =
	Mapping::LookupTable_t<50>::generate<VoltageToFreqTableRange>([](float x) { return x * x + 7; });

TEST_CASE("check mapping table") {
	constexpr unsigned LEN = 50;
	constexpr float min = -0.1f;
	constexpr float max = 0.5f;

	// generate check data with original table:
	// for (unsigned i = 0; i < LEN; i++) {
	// 	auto x = min + i * (max - min) / float(LEN - 1);
	// 	std::cout << "\t{" << x << ", " << VoltageToFrequencyTable.lookup(x) << "},\n";
	// }
	constexpr std::pair<float, float> checkdata[] = {
		{-0.1, 7.01},		  {-0.0877551, 7.0077}, {-0.0755102, 7.0057}, {-0.0632653, 7.004}, {-0.0510204, 7.0026},
		{-0.0387755, 7.0015}, {-0.0265306, 7.0007}, {-0.0142857, 7.0002}, {-0.00204081, 7},	   {0.0102041, 7.0001},
		{0.022449, 7.0005},	  {0.0346939, 7.0012},	{0.0469388, 7.0022},  {0.0591837, 7.0035}, {0.0714286, 7.0051},
		{0.0836735, 7.007},	  {0.0959184, 7.0092},	{0.108163, 7.0117},	  {0.120408, 7.0145},  {0.132653, 7.0176},
		{0.144898, 7.021},	  {0.157143, 7.02469},	{0.169388, 7.02869},  {0.181633, 7.03299}, {0.193878, 7.03759},
		{0.206122, 7.04249},  {0.218367, 7.04768},	{0.230612, 7.05318},  {0.242857, 7.05898}, {0.255102, 7.06508},
		{0.267347, 7.07147},  {0.279592, 7.07817},	{0.291837, 7.08517},  {0.304082, 7.09247}, {0.316327, 7.10006},
		{0.328571, 7.10796},  {0.340816, 7.11616},	{0.353061, 7.12465},  {0.365306, 7.13345}, {0.377551, 7.14254},
		{0.389796, 7.15194},  {0.402041, 7.16164},	{0.414286, 7.17163},  {0.426531, 7.18193}, {0.438776, 7.19252},
		{0.45102, 7.20342},	  {0.463265, 7.21461},	{0.47551, 7.22611},	  {0.487755, 7.23791}, {0.5, 7.25},
	};

	// Check data with refactored table:
	for (unsigned i = 0; i < LEN; i++) {
		auto x = min + i * (max - min) / float(LEN - 1);
		auto y = VoltageToFrequencyTable.lookup(x);
		CHECK(y == doctest::Approx(checkdata[i].second));
	}
}
