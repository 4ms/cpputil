#include "doctest.h"
#include "util/term_codes.hh"
#include <cstring>

TEST_CASE("Term: Normal code") {
	CHECK(std::strcmp(Term::Normal, "\x1b[0m") == 0);
}

TEST_CASE("Term: basic color codes") {
	CHECK(std::strcmp(Term::Black, "\x1b[30m") == 0);
	CHECK(std::strcmp(Term::Red, "\x1b[31m") == 0);
	CHECK(std::strcmp(Term::Green, "\x1b[32m") == 0);
	CHECK(std::strcmp(Term::Yellow, "\x1b[33m") == 0);
	CHECK(std::strcmp(Term::Blue, "\x1b[34m") == 0);
	CHECK(std::strcmp(Term::Magenta, "\x1b[35m") == 0);
	CHECK(std::strcmp(Term::Cyan, "\x1b[36m") == 0);
	CHECK(std::strcmp(Term::White, "\x1b[37m") == 0);
}

TEST_CASE("Term: bold color codes") {
	CHECK(std::strcmp(Term::BoldRed, "\x1b[31;1m") == 0);
	CHECK(std::strcmp(Term::BoldGreen, "\x1b[32;1m") == 0);
	CHECK(std::strcmp(Term::Bold, "\x1b[1m") == 0);
}

TEST_CASE("Term: blink codes") {
	CHECK(std::strcmp(Term::BlinkRed, "\x1b[31;5m") == 0);
	CHECK(std::strcmp(Term::Blink, "\x1b[5m") == 0);
}

TEST_CASE("Term: cursor control codes") {
	CHECK(std::strcmp(Term::JumpLineStart, "\x1b[0G") == 0);
	CHECK(std::strcmp(Term::ClearLine, "\x1b[2K") == 0);
	CHECK(std::strcmp(Term::ClearScreen, "\x1b[2J") == 0);
	CHECK(std::strcmp(Term::SaveCursorPos, "\x1b[s") == 0);
	CHECK(std::strcmp(Term::RestoreCursorPos, "\x1b[u") == 0);
}

TEST_CASE("Term: all codes start with ESC") {
	// ESC is 0x1b
	CHECK(Term::Normal[0] == '\x1b');
	CHECK(Term::Red[0] == '\x1b');
	CHECK(Term::Bold[0] == '\x1b');
}
