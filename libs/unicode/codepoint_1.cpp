
#include <iostream>
#include <string>

#include <lulib/unicode/codepoint.hpp>

void func(lulib::codepoint &cp) {
	if (cp) {
		std::cout << "codepoint valid." << std::endl;
	}
	else {
		std::cout << "codepoint invalid." << std::endl;
	}

	std::cout << std::dec;
	//std::cout << " bit size: " << cp.bit_size() << std::endl;
	std::cout << "  " << "byte size (UTF-8  require): " << cp.utf8_size() << std::endl;
	std::cout << "  " << "byte size (UTF-16 require): " << cp.utf16_size() << std::endl;
	std::cout << "  " << std::hex;
	std::cout << "  " << "codepoint: 0x" << cp << std::endl;

}

int main() {
	lulib::codepoint cp;

	// code-point 直接指定
	cp = 0x12345678;
	func(cp);
	cp = 0x6f22;  // 漢字
	func(cp);

	// 文字列解析
	{
		std::string str(u8"あ");
		cp << str;
		func(cp);
	}
	{
		std::string str(u8"いうえお");
		cp << str;
		func(cp);
	}
	{
		cp << u8"a";
		func(cp);
	}
	{
		cp << u8"bcde";
		func(cp);
	}

}


