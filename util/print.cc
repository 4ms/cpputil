#include "print.hh"
#include <iostream>
#include <span>

template<unsigned Place>
char digit(int &value) {
	if (value > Place) {
		unsigned dig = value / Place;
		value -= dig * Place;
		return '0' + dig;
	}
	return 0;
}

void int_to_str(int value, const std::span<char> buf) {
	if (buf.size() <= 1)
		return;

	if (value == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	int len = 0;
	if (value < 0) {
		buf[len++] = '-';
		value = -value;
	}

	if (char dig = digit<10000>(value); dig)
		buf[len++] = dig;
	if (char dig = digit<1000>(value); dig)
		buf[len++] = dig;
	if (char dig = digit<100>(value); dig)
		buf[len++] = dig;
	if (char dig = digit<10>(value); dig)
		buf[len++] = dig;
	if (char dig = digit<1>(value); dig)
		buf[len++] = dig;

	buf[len] = '\0';
}

// #Method #1:
// putchar_s(const char) Must be defined in the applicaton code somewhere
// void putchar_s(const char c);

// Method #2:
// using putchar_t = void (*)(const char c);
// static putchar_t putchar_s;
// void register_putchar(putchar_t func) {
// 	putchar_s = func;
// }

// Method #3:
// templates in .hh

// void printone(const unsigned char *str) {
// 	while (*str)
// 		putchar_s(*str++);
// }

// void printone(const char *str) {
// 	while (*str)
// 		putchar_s(*str++);
// }

// void printone(Hex hex) {
// 	auto value = hex.x;

// 	if (!value) {
// 		printone("0");
// 		return;
// 	}
// 	/// TODO: read left to right, not right to left, to avoid flipping digits
// 	constexpr int MAX_DIGITS = 8;
// 	char buf[MAX_DIGITS + 1];
// 	int len = 0;
// 	do {
// 		const char digit = (char)(value & 0xF);
// 		buf[len++] = digit < 10 ? '0' + digit : 'A' + digit - 10;
// 		value >>= 4;
// 	} while (value && (len < MAX_DIGITS));
// 	buf[len] = '\0';

// 	for (int i = 0; i < len / 2; i++) {
// 		auto tmp = buf[i];
// 		buf[i] = buf[len - i - 1];
// 		buf[len - i - 1] = tmp;
// 	}

// 	printone(buf);
// }

// void printone(int value) {
// 	if (!value) {
// 		printone("0");
// 		return;
// 	}

// 	if (value < 0) {
// 		printone("-");
// 		value = -value;
// 	}

// 	constexpr int MAX_DIGITS = 10;
// 	char buf[MAX_DIGITS + 1];
// 	int len = 0;
// 	do {
// 		const char digit = (char)(value % 10);
// 		buf[len++] = '0' + digit;
// 		value /= 10;
// 	} while (value && (len < MAX_DIGITS));
// 	buf[len] = '\0';

// 	for (int i = 0; i < len / 2; i++) {
// 		auto tmp = buf[i];
// 		buf[i] = buf[len - i - 1];
// 		buf[len - i - 1] = tmp;
// 	}

// 	printone(buf);
// }
