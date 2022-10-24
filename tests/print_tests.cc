#include "doctest.h"
#include "util/print.hh"
#include <span>

unsigned num_chars_printed = 0;
char buffer[128]{};

void putc(const char c) {
	buffer[num_chars_printed] = c;
	num_chars_printed++;
}

// template<putchar_t P = putc>
// using _print_ = print<P>;

TEST_CASE("print() usage") {
	num_chars_printed = 0;
	print<putc>("Hi");
	CHECK(num_chars_printed == 2);
	CHECK(buffer[0] == 'H');
	CHECK(buffer[1] == 'i');

	num_chars_printed = 0;
	printone<putc>("Test");
	CHECK(num_chars_printed == 4);
	CHECK(buffer[0] == 'T');
	CHECK(buffer[1] == 'e');
	CHECK(buffer[2] == 's');
	CHECK(buffer[3] == 't');
}

unsigned num_chars_logged = 0;
void log(auto... x) {
	(print<[](const char c) { num_chars_logged++; }>(x), ...);
}

unsigned num_chars_warned = 0;
void warn(auto... x) {
	(print<[](const char c) { num_chars_warned++; }>(x), ...);
}

TEST_CASE("wrapper method usage") {
	num_chars_logged = 0;
	num_chars_warned = 0;

	SUBCASE("use log()") {
		log("One", "2", "Three");
		CHECK(num_chars_logged == 9);
		CHECK(num_chars_warned == 0);
	}

	SUBCASE("use warn()") {
		warn("e", "2", "Three");
		CHECK(num_chars_logged == 0);
		CHECK(num_chars_warned == 7);
	}
}
