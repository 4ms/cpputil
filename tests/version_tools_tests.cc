#include "doctest.h"
#include "util/version_tools.hh"

TEST_CASE("parse") {
	{
		auto vers = VersionUtil::parse_version("1.2.16");
		CHECK(vers.major == 1);
		CHECK(vers.minor == 2);
		CHECK(vers.revision == 16);
	}

	{
		auto vers = VersionUtil::parse_version("9.1");
		CHECK(vers.major == 9);
		CHECK(vers.minor == 1);
		CHECK(vers.revision == 0);
	}

	{
		auto vers = VersionUtil::parse_version("2.0.99");
		CHECK(vers.major == 2);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 99);
	}

	{
		auto vers = VersionUtil::parse_version("2099");
		CHECK(vers.major == 2099);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 0);
	}

	{
		auto vers = VersionUtil::parse_version("garbage");
		CHECK(vers.major == 0);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 0);
		// try {
		// 	auto vers = VersionUtil::parse_version("garbage");
		// 	(void)vers;
		// 	CHECK(false);
		// } catch (std::exception) {
		// 	CHECK(true);
		// }
	}
}
