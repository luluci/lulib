#include <iostream>
#include <string>
#include <typeinfo>

#include <lulib/lexical_cast.hpp>
#include <lulib/type_info.hpp>

template<typename T, typename V1, std::size_t N1, typename V2, std::size_t N2>
void test_excep(V1 (&prefix)[N1], V2 (&str)[N2]) {
	try {
		auto result = lulib::lexical_cast<T>(str);
		std::cout << "lexical_cast success: " << std::endl;
		std::cout << "    " << prefix << str << " -> " << result
			<< " (" << lulib::type_info<decltype( lulib::lexical_cast<T>(str) )>().name() << ")" << std::endl;
	} catch (lulib::bad_lexical_cast &e) {
		std::cout << "lexical_cast failed: " << std::endl;
		std::cout << "    src: " << e.source() << std::endl;
	}
}
template<typename T, typename V1, std::size_t N1, typename V2, std::size_t N2>
void test_opt(V1 (&prefix)[N1], V2 (&str)[N2]) {
	if (auto result = lulib::lexical_cast<T>(str)) {
		std::cout << "lexical_cast success: " << std::endl;
		std::cout << "    " << prefix << str << " -> " << *result
			<< " (" << lulib::type_info<decltype( lulib::lexical_cast<T>(str) )>().name() << ")" << std::endl;
	}
	else {
	}
}


int main() {
	test_excep<int>("int: ", "12345");
	test_excep<int>("int: ", "-6789");
	test_excep<int>("int: ", "FFFF");
	test_excep<unsigned int>("uint: ", "12345");
	test_excep<unsigned int>("uint: ", "FFFF");
	test_excep<unsigned int>("uint: ", "11101001001");
	test_excep<unsigned int>("uint: ", "02412");
	test_excep<lulib::as_hex>("hex: ", "1234");
	test_excep<lulib::as_hex>("hex: ", "FFFF");
	test_excep<lulib::as_hex>("hex: ", "FFFFFFFF");
	test_excep<lulib::as_hex>("hex: ", "FFFFFFFFFFFF");
	test_excep<lulib::as_hex64>("hex: ", "FFFFFFFFFFFF");
	test_excep<lulib::as_oct>("oct: ", "1234");
	test_excep<lulib::as_oct>("oct: ", "5671234567");
	test_excep<lulib::as_oct>("oct: ", "12348");
	test_excep<lulib::as_bin>("bin: ", "00001111");
	test_excep<lulib::as_bin>("bin: ", "11111111");
	test_excep<lulib::as_bin>("bin: ", "111111112");
	test_excep<short>("short: ", "0.111111111111111111111");
	test_excep<double>("double: ", "0.111111111111111111111");

	test_opt< boost::optional<int> >("int: ", "12345");
}

