#include <iostream>
#include <string>
#include <typeinfo>

#include <lulib/lexical_cast.hpp>
#include <lulib/type_info.hpp>

template<typename T, typename V1, std::size_t N1, typename V2, std::size_t N2>
void test(V1 (&prefix)[N1], V2 (&str)[N2]) {
	auto result = lulib::lexical_cast<T>(str);
	std::cout << prefix
		<< str << " -> "
		<< "(" << lulib::type_info<decltype( lulib::lexical_cast<T>(str) )>().name() << ")"
		<< result << std::endl;
}


int main() {
	test<int>("int: ", "12345");
	test<int>("int: ", "-6789");
	test<int>("int: ", "FFFF");
	test<unsigned int>("uint: ", "12345");
	test<unsigned int>("uint: ", "FFFF");
	test<unsigned int>("uint: ", "11101001001");
	test<unsigned int>("uint: ", "02412");
	test<lulib::hex>("hex: ", "1234");
	test<lulib::hex>("hex: ", "FFFF");
	test<lulib::hex>("hex: ", "FFFFFFFF");
	test<lulib::hex>("hex: ", "FFFFFFFFFFFF");
	test<lulib::hex64_t>("hex: ", "FFFFFFFFFFFF");
	test<lulib::oct>("oct: ", "1234");
	test<lulib::oct>("oct: ", "5671234567");
	test<lulib::oct>("oct: ", "12348");
	test<lulib::bin>("bin: ", "00001111");
	test<lulib::bin>("bin: ", "11111111");
	test<lulib::bin>("bin: ", "111111112");
	test<short>("short: ", "0.111111111111111111111");
	test<double>("double: ", "0.111111111111111111111");
}

