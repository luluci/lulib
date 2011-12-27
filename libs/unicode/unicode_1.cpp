
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

#include <boost/foreach.hpp>

#include <lulib/unicode/unicode.hpp>

template<typename String>
void func1(String &&str) {
	lulib::unicode uc;
	uc.convert(str);

	std::cout << "** UTF-8 **" << std::endl;
	std::cout << std::dec;
	std::cout << "  UTF-8  size: " << uc.utf8_size() << std::endl;
	std::cout << "  UTF-16 size: " << uc.utf16_size() << std::endl;
	std::cout << "  UTF-32 size: " << uc.utf32_size() << std::endl;
	std::cout << std::hex;
	std::cout << "  Code Point sequence: ";
	BOOST_FOREACH( auto &cp, uc ) {
		std::cout << cp.value() << " ";
	}
	std::cout << std::endl;

	BOOST_FOREACH( char ch, uc.utf8() ) {
		std::cout << ch;
	}
	std::cout << std::endl;
}

template<typename String>
void func2(String &&str) {
	lulib::unicode uc;
	uc.convert( std::begin(str), std::end(str) );

	BOOST_FOREACH( char16_t ch, uc.utf16() ) {
		fprintf(stdout, "%c", (char)((ch & 0xFF00) >> 8));
		fprintf(stdout, "%c", (char)(ch & 0x00FF));
	}
	fprintf(stdout, "%c%c", '\0', '\n');
}

template<typename String>
void func3(String &&str) {
	lulib::unicode uc;
	uc.convert( std::begin(str), std::end(str) );

	BOOST_FOREACH( char32_t ch, uc.utf32() ) {
		fprintf(stdout, "%c", (char)((ch & 0xFF00) >> 8));
		fprintf(stdout, "%c", (char)(ch & 0x00FF));
	}
	fprintf(stdout, "%c%c", '\0', '\n');
}

#include <sstream>

int main(int argc, char *argv[]) {
	int unicode = 0;
	if (argc >= 2) {
		std::stringstream ss;
		ss << argv[1];
		ss >> unicode;
	}

	std::string str1("abcde");
	char str2[] = "あいうえお";
	char str3[] = "漢字";

	switch (unicode) {
		case 0: { func1(str1); func1(str2); func1(str3); break; }  // UTF-8
		case 1: { func2(str1); func2(str2); func2(str3); break; }  // UTF-16
		case 2: { func3(str1); func3(str2); func3(str3); break; }  // UTF-32
	}
}


