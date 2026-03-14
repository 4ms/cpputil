#include "doctest.h"
#include "util/string_util.hh"

TEST_CASE("ReplaceString: no match leaves string unchanged") {
	ReplaceString rs{"hello world"};
	rs.replace_all("xyz", "abc");
	CHECK(rs.str == "hello world");
}

TEST_CASE("ReplaceString: single replacement") {
	ReplaceString rs{"hello world"};
	rs.replace_all("world", "there");
	CHECK(rs.str == "hello there");
}

TEST_CASE("ReplaceString: multiple occurrences replaced") {
	ReplaceString rs{"aabbaa"};
	rs.replace_all("aa", "xx");
	CHECK(rs.str == "xxbbxx");
}

TEST_CASE("ReplaceString: replace with empty string (deletion)") {
	ReplaceString rs{"hello world"};
	rs.replace_all(" ", "");
	CHECK(rs.str == "helloworld");
}

TEST_CASE("ReplaceString: replace with longer non-overlapping string") {
	// Note: searching for "a" and replacing with "aaa" would loop infinitely
	// because the replacement contains the search term. Use a safe replacement.
	ReplaceString rs{"xbx"};
	rs.replace_all("x", "yy");
	CHECK(rs.str == "yybyy");
}

TEST_CASE("ReplaceString: chaining replacements") {
	ReplaceString rs{"hello world"};
	rs.replace_all("hello", "hi").replace_all("world", "there");
	CHECK(rs.str == "hi there");
}

TEST_CASE("ReplaceString: empty input string") {
	ReplaceString rs{""};
	rs.replace_all("a", "b");
	CHECK(rs.str == "");
}

TEST_CASE("tokenize_string: single token") {
	std::vector<std::string> tokens;
	tokenize_string("hello", tokens);
	REQUIRE(tokens.size() == 1);
	CHECK(tokens[0] == "hello");
}

TEST_CASE("tokenize_string: two tokens with comma delimiter") {
	std::vector<std::string> tokens;
	tokenize_string("hello,world", tokens);
	REQUIRE(tokens.size() == 2);
	CHECK(tokens[0] == "hello");
	CHECK(tokens[1] == "world");
}

TEST_CASE("tokenize_string: multiple tokens") {
	std::vector<std::string> tokens;
	tokenize_string("a,b,c,d", tokens);
	REQUIRE(tokens.size() == 4);
	CHECK(tokens[0] == "a");
	CHECK(tokens[3] == "d");
}

TEST_CASE("tokenize_string: custom delimiter") {
	std::vector<std::string> tokens;
	tokenize_string("a:b:c", tokens, ":");
	REQUIRE(tokens.size() == 3);
	CHECK(tokens[1] == "b");
}

TEST_CASE("tokenize_string: leading delimiter skipped") {
	std::vector<std::string> tokens;
	tokenize_string(",hello,world", tokens);
	REQUIRE(tokens.size() == 2);
	CHECK(tokens[0] == "hello");
}

TEST_CASE("tokenize_string: empty string produces no tokens") {
	std::vector<std::string> tokens;
	tokenize_string("", tokens);
	CHECK(tokens.empty());
}
