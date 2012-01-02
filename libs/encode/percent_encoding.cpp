#include <string>
#include <iostream>

#include <lulib/encode.hpp>

template<typename String>
void func(String &&str) {
	typedef lulib::encode_system::percent_encoding encoder;
	encoder::result_type result = lulib::encode<encoder>(str);
	std::cout << str << " => " << result << std::endl;
}
template<typename T, std::size_t N>
void func(T (&str)[N]) {
	typedef lulib::encode_system::percent_encoding encoder;
	encoder::result_type result = lulib::encode<encoder>(str);
	std::cout << str << " => " << result << std::endl;
}

int main() {
	std::string str = "ウィキペディア";
	func(str);
	func("http://www.test/~test/");
}
