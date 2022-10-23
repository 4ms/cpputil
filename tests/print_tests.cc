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

TEST_CASE("int to str") {
	char b[5] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F};
	std::span<char, 5> buf{b};

	SUBCASE("Basic usage") {
		int_to_str(123, buf);
		CHECK(buf[0] == '1');
		CHECK(buf[1] == '2');
		CHECK(buf[2] == '3');
		CHECK(buf[3] == '\0');
		CHECK(buf[4] == 0x7F);
	}

	SUBCASE("Overflow drops leading digits") {
		int_to_str(12345, buf);
		//The '1' is dropped
		CHECK(buf[0] == '1');
		CHECK(buf[1] == '2');
		CHECK(buf[2] == '3');
		CHECK(buf[3] == '4');
		CHECK(buf[4] == '5');
	}

	SUBCASE("Negative") {
		int_to_str(-123, buf);
		CHECK(buf[0] == '-');
		CHECK(buf[1] == '1'); //3
		CHECK(buf[2] == '2'); //2
		CHECK(buf[3] == '3'); // \0
		CHECK(buf[4] == '\0');
		//int_to_str(-12345, buf);
		////The '123' is dropped and the - is ended
		//CHECK(buf[0] == '4');
		//CHECK(buf[1] == '5');
		//CHECK(buf[2] == '-');
		//CHECK(buf[3] == '\0');
		//CHECK(buf[4] == 0x7F);
	}
}
