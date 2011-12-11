
#include <iostream>
#include <string>
#include <vector>

#include <lulib/type_traits/char_traits.hpp>
#include <lulib/type_info.hpp>

template<typename T>
void func() {
	typedef lulib::char_traits<T> traits;
	std::cout << "  char_type: " << lulib::type_info< typename traits::char_type >().name() << std::endl;
	std::cout << "string_type: " << lulib::type_info< typename traits::string_type >().name() << std::endl;
}

int main() {
	func< char >();
	func< wchar_t >();
	func< char16_t >();
	func< char32_t >();
	func< std::string >();
	func< std::wstring >();
	func< std::u16string >();
	func< std::u32string >();
	func< std::vector<char> >();
	func< std::vector<wchar_t> >();
	func< std::vector<char16_t> >();
	func< std::vector<char32_t> >();
}
