#pragma once
#include <span>

struct Hex {
	unsigned x;
};

void printone(const unsigned char *str);
void printone(const char *str);
void printone(int value);
void printone(Hex value);

template<typename... Types>
void print(Types... args) {
	(printone(args), ...);
}

/////////// Alt method: specify putc as a template parameter

using putchar_t = void (*)(const char c);

template<putchar_t putc>
void printone(const char *str) {
	while (*str)
		putc(*str++);
}

template<putchar_t putc>
void printone(const unsigned char *str) {
	while (*str)
		putc(*str++);
}

template<putchar_t putc>
void printone(Hex hex) {
	auto value = hex.x;

	if (!value) {
		putc('0');
		return;
	}
	/// TODO: read left to right, not right to left, to avoid flipping digits
	constexpr int MAX_DIGITS = 8;
	char buf[MAX_DIGITS + 1];
	int len = 0;
	do {
		const char digit = (char)(value & 0xF);
		buf[len++] = digit < 10 ? '0' + digit : 'A' + digit - 10;
		value >>= 4;
	} while (value && (len < MAX_DIGITS));
	buf[len] = '\0';

	for (int i = 0; i < len / 2; i++) {
		auto tmp = buf[i];
		buf[i] = buf[len - i - 1];
		buf[len - i - 1] = tmp;
	}

	printone<putc>(buf);
}

template<putchar_t putc>
void printone(int value) {
	if (!value) {
		putc('0');
		return;
	}

	if (value < 0) {
		putc('-');
		value = -value;
	}

	constexpr int MAX_DIGITS = 10;
	char buf[MAX_DIGITS + 1];
	int len = 0;
	do {
		const char digit = (char)(value % 10);
		buf[len++] = '0' + digit;
		value /= 10;
	} while (value && (len < MAX_DIGITS));
	buf[len] = '\0';

	for (int i = 0; i < len / 2; i++) {
		auto tmp = buf[i];
		buf[i] = buf[len - i - 1];
		buf[len - i - 1] = tmp;
	}

	printone<putc>(buf);
}

template<putchar_t putc, typename... Types>
void print(Types... args) {
	(printone<putc>(args), ...);
}
