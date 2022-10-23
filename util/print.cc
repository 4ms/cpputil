#include "print.hh"
#include <iostream>
#include <span>

void int_to_str(int value, const std::span<char> buf) {
	if (buf.size() <= 1)
		return;

	if (value == 0) {
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	int len = 0;
	unsigned start = 0;
	if (value < 0) {
		start = 1;
		buf[len++] = '-';
		value = -value;
	}

	//save 1 char for the zero terminator, and 1 for the - sign if negative
	const int MAX_DIGITS = buf.size() - 1;
	std::cout << "MAX: " << MAX_DIGITS << std::endl;

	do {
		const char digit = (char)(value % 10);
		buf[len++] = '0' + digit;
		value /= 10;
		std::cout << "dig = " << '0' + digit << " value now " << value << " len: " << len << std::endl;
	} while (value && (len <= MAX_DIGITS));
	buf[len] = '\0';

	std::cout << "len: " << len << " start: " << start << " buf: " << buf.data() << std::endl;
	for (int i = start; i < (len - start) / 2; i++) {
		std::swap(buf[i], buf[len - start - i - 1]);
	}
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
