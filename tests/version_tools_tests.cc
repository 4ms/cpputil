#include "doctest.h"
#include "util/version_tools.hh"

TEST_CASE("parse") {
	{
		auto vers = VersionUtil::Version("1.2.16");
		CHECK(vers.major == 1);
		CHECK(vers.minor == 2);
		CHECK(vers.revision == 16);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("9.1");
		CHECK(vers.major == 9);
		CHECK(vers.minor == 1);
		CHECK(vers.revision == 0);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("2.0.99");
		CHECK(vers.major == 2);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 99);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("209");
		CHECK(vers.major == 209);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 0);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("garbage");
		CHECK(vers.major == 0);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 0);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("67.BAD.123");
		CHECK(vers.major == 67);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 123);
		CHECK(vers.is_normal);
	}

	{
		auto vers = VersionUtil::Version("77.-22.33");
		CHECK(vers.major == 77);
		CHECK(vers.minor == 0);
		CHECK(vers.revision == 33);
		CHECK(vers.is_normal);
	}
	{
		auto vers = VersionUtil::Version("1.2.3-dev");
		CHECK(vers.major == 1);
		CHECK(vers.minor == 2);
		CHECK(vers.revision == 3);
		CHECK_FALSE(vers.is_normal);
	}
	{
		auto vers = VersionUtil::Version("1.2.3");
		CHECK(vers.is_normal);
	}
	{
		auto vers = VersionUtil::Version("1.2.3-");
		CHECK_FALSE(vers.is_normal);
	}
}

TEST_CASE("operator==") {
	CHECK(VersionUtil::Version{1, 2, 3} == VersionUtil::Version{"1.2.3"});
	CHECK(VersionUtil::Version{"001.02.00000003"} == VersionUtil::Version{"1.2.3"});
}

TEST_CASE("can host") {
	using namespace VersionUtil;
	// rev can be same, equal, greater
	CHECK(Version{"1.2.2"}.can_host_version(Version{"1.2.3"}));
	CHECK(Version{"1.2.3"}.can_host_version(Version{"1.2.3"}));
	CHECK(Version{"1.2.4"}.can_host_version(Version{"1.2.3"}));

	// Minor can be greater
	CHECK(Version{"1.3.2"}.can_host_version(Version{"1.2.3"}));
	CHECK(Version{"1.3.3"}.can_host_version(Version{"1.2.3"}));
	CHECK(Version{"1.3.4"}.can_host_version(Version{"1.2.3"}));

	// Major cannot be less
	CHECK_FALSE(Version{"0.9.9"}.can_host_version(Version{"1.2.3"}));
	CHECK_FALSE(Version{"0.2.3"}.can_host_version(Version{"1.2.3"}));
	CHECK_FALSE(Version{"0.1.1"}.can_host_version(Version{"1.2.3"}));

	// Major cannot be greater
	CHECK_FALSE(Version{"2.2.3"}.can_host_version(Version{"1.2.3"}));
	CHECK_FALSE(Version{"2.0.0"}.can_host_version(Version{"1.2.3"}));
	CHECK_FALSE(Version{"2.9.9"}.can_host_version(Version{"1.2.3"}));
}

TEST_CASE("operators <=> and ==") {
	using namespace VersionUtil;
	CHECK(Version{"1.2.2"} < Version{"1.2.3"});
	CHECK(Version{"1.2.3"} == (Version{"1.2.3"}));
	CHECK(Version{"1.2.3"} >= (Version{"1.2.3"}));
	CHECK(Version{"1.2.4"} > (Version{"1.2.3"}));
	CHECK(Version{"1.2.4"} >= (Version{"1.2.3"}));

	CHECK(Version{"1.3.2"} > (Version{"1.2.3"}));
	CHECK(Version{"1.3.3"} > (Version{"1.2.3"}));
	CHECK(Version{"1.3.4"} > (Version{"1.2.3"}));

	CHECK(Version{"0.9.9"} < (Version{"1.2.3"}));
	CHECK(Version{"0.2.3"} < (Version{"1.2.3"}));
	CHECK(Version{"0.1.1"} < (Version{"1.2.3"}));

	CHECK(Version{"2.0.0"} > (Version{"1.2.3"}));
	CHECK(Version{"2.2.3"} > (Version{"1.2.3"}));
	CHECK(Version{"2.9.9"} > (Version{"1.2.3"}));
}
