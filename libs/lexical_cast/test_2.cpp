#include <iostream>
#include <string>
#include <typeinfo>

#include <lulib/lexical_cast.hpp>
#include <lulib/type_info.hpp>

template<typename T, typename V1, std::size_t N1, typename V2, std::size_t N2>
void test_excep(V1 (&prefix)[N1], V2 (&str)[N2]) {
	try {
		auto result = lulib::lexical_cast<T>(str);
		std::cout << "excep ver: success: ";
		std::cout << prefix << str << " -> " << result
			<< " (" << lulib::type_info<decltype( lulib::lexical_cast<T>(str) )>().name() << ")" << std::endl;
	} catch (lulib::bad_lexical_cast &e) {
		std::cout << "excep ver:  failed: ";
		std::cout << prefix << str << " -> *" << std::endl;
	}
}
template<typename T, typename V1, std::size_t N1, typename V2, std::size_t N2>
void test_opt(V1 (&prefix)[N1], V2 (&str)[N2]) {
	if (auto result = lulib::lexical_cast<T>(str)) {
		std::cout << "  opt ver: success: ";
		std::cout << prefix << str << " -> " << *result
			<< " (" << lulib::type_info<decltype( lulib::lexical_cast<T>(str) )>().name() << ")" << std::endl;
	}
	else {
		std::cout << "  opt ver:  failed: ";
		std::cout << prefix << str << " -> *" << std::endl;
	}
}

void test1() {
	// 8bit: char
	test_excep<char>		("       char: ", "-129");
	test_excep<char>		("       char: ", "-128");
	test_excep<char>		("       char: ", "127");
	test_excep<char>		("       char: ", "128");
	test_excep<signed char>		("      schar: ", "-129");
	test_excep<signed char>		("      schar: ", "-128");
	test_excep<signed char>		("      schar: ", "127");
	test_excep<signed char>		("      schar: ", "128");
	test_excep<unsigned char>	("      uchar: ", "-1");
	test_excep<unsigned char>	("      uchar: ", "0");
	test_excep<unsigned char>	("      uchar: ", "255");
	test_excep<unsigned char>	("      uchar: ", "256");

	// 16bit: short
	test_excep< lulib::as_int16 >	("      short: ", "-32769");
	test_excep< lulib::as_int16 >	("      short: ", "-32768");
	test_excep< lulib::as_int16 >	("      short: ", "32767");
	test_excep< lulib::as_int16 >	("      short: ", "32768");
	test_excep< lulib::as_uint16 >	("     ushort: ", "-1");
	test_excep< lulib::as_uint16 >	("     ushort: ", "0");
	test_excep< lulib::as_uint16 >	("     ushort: ", "65535");
	test_excep< lulib::as_uint16 >	("     ushort: ", "65536");

	// 32bit: int
	test_excep<int>			("        int: ", "-2147483649");
	test_excep<int>			("        int: ", "-2147483648");
	test_excep<int>			("        int: ", "2147483647");
	test_excep<int>			("        int: ", "2147483648");
	test_excep<unsigned int>	("       uint: ", "-1");
	test_excep<unsigned int>	("       uint: ", "0");
	test_excep<unsigned int>	("       uint: ", "4294967295");
	test_excep<unsigned int>	("       uint: ", "4294967296");

	// 64bit: long long
	test_excep<lulib::as_int64>	("  long long: ", "-9223372036854775809");
	test_excep<lulib::as_int64>	("  long long: ", "-9223372036854775808");
	test_excep<lulib::as_int64>	("  long long: ", "9223372036854775807");
	test_excep<lulib::as_int64>	("  long long: ", "9223372036854775808");
	test_excep<lulib::as_uint64>	("u long long: ", "-1");
	test_excep<lulib::as_uint64>	("u long long: ", "0");
	test_excep<lulib::as_uint64>	("u long long: ", "18446744073709551615");
	test_excep<lulib::as_uint64>	("u long long: ", "18446744073709551616");

	// float
	// double

	// hex 8bit
	test_excep<lulib::as_hex8>	("       hex8: ",  "00");
	test_excep<lulib::as_hex8>	("       hex8: ",  "7F");
	test_excep<lulib::as_hex8>	("       hex8: ",  "80");
	test_excep<lulib::as_hex8>	("       hex8: ",  "FF");
	test_excep<lulib::as_hex8>	("       hex8: ", "100");
	// hex 16bit
	test_excep<lulib::as_hex16>	("      hex16: ",  "0000");
	test_excep<lulib::as_hex16>	("      hex16: ",  "7FFF");
	test_excep<lulib::as_hex16>	("      hex16: ",  "8000");
	test_excep<lulib::as_hex16>	("      hex16: ",  "FFFF");
	test_excep<lulib::as_hex16>	("      hex16: ", "10000");
	// hex 32bit
	test_excep<lulib::as_hex32>	("      hex32: ",  "00000000");
	test_excep<lulib::as_hex32>	("      hex32: ",  "7FFFFFFF");
	test_excep<lulib::as_hex32>	("      hex32: ",  "80000000");
	test_excep<lulib::as_hex32>	("      hex32: ",  "FFFFFFFF");
	test_excep<lulib::as_hex32>	("      hex32: ", "100000000");
	// hex 64bit
	test_excep<lulib::as_hex64>	("      hex64: ",  "0000000000000000");
	test_excep<lulib::as_hex64>	("      hex64: ",  "7FFFFFFFFFFFFFFF");
	test_excep<lulib::as_hex64>	("      hex64: ",  "8000000000000000");
	test_excep<lulib::as_hex64>	("      hex64: ",  "FFFFFFFFFFFFFFFF");
	test_excep<lulib::as_hex64>	("      hex64: ", "10000000000000000");

}

void test2() {
	// 8bit: char
	test_opt< boost::optional<char> >		("       char: ", "-129");
	test_opt< boost::optional<char> >		("       char: ", "-128");
	test_opt< boost::optional<char> >		("       char: ", "127");
	test_opt< boost::optional<char> >		("       char: ", "128");
	test_opt< boost::optional<signed char> >	("      schar: ", "-129");
	test_opt< boost::optional<signed char> >	("      schar: ", "-128");
	test_opt< boost::optional<signed char> >	("      schar: ", "127");
	test_opt< boost::optional<signed char> >	("      schar: ", "128");
	test_opt< boost::optional<unsigned char> >	("      uchar: ", "-1");
	test_opt< boost::optional<unsigned char> >	("      uchar: ", "0");
	test_opt< boost::optional<unsigned char> >	("      uchar: ", "255");
	test_opt< boost::optional<unsigned char> >	("      uchar: ", "256");

	// 16bit: short
	test_opt< lulib::as_int16_opt >			("      short: ", "-32769");
	test_opt< lulib::as_int16_opt >			("      short: ", "-32768");
	test_opt< lulib::as_int16_opt >			("      short: ", "32767");
	test_opt< lulib::as_int16_opt >			("      short: ", "32768");
	test_opt< lulib::as_uint16_opt >		("     ushort: ", "-1");
	test_opt< lulib::as_uint16_opt >		("     ushort: ", "0");
	test_opt< lulib::as_uint16_opt >		("     ushort: ", "65535");
	test_opt< lulib::as_uint16_opt >		("     ushort: ", "65536");

	// 32bit: int
	test_opt< boost::optional<int> >		("        int: ", "-2147483649");
	test_opt< boost::optional<int> >		("        int: ", "-2147483648");
	test_opt< boost::optional<int> >		("        int: ", "2147483647");
	test_opt< boost::optional<int> >		("        int: ", "2147483648");
	test_opt< boost::optional<unsigned int> >	("       uint: ", "-1");
	test_opt< boost::optional<unsigned int> >	("       uint: ", "0");
	test_opt< boost::optional<unsigned int> >	("       uint: ", "4294967295");
	test_opt< boost::optional<unsigned int> >	("       uint: ", "4294967296");

	// 64bit: long long
	test_opt<lulib::as_int64_opt>			("  long long: ", "-9223372036854775809");
	test_opt<lulib::as_int64_opt>			("  long long: ", "-9223372036854775808");
	test_opt<lulib::as_int64_opt>			("  long long: ", "9223372036854775807");
	test_opt<lulib::as_int64_opt>			("  long long: ", "9223372036854775808");
	test_opt<lulib::as_uint64_opt>			("u long long: ", "-1");
	test_opt<lulib::as_uint64_opt>			("u long long: ", "0");
	test_opt<lulib::as_uint64_opt>			("u long long: ", "18446744073709551615");
	test_opt<lulib::as_uint64_opt>			("u long long: ", "18446744073709551616");

	// float
	// double

	// hex 8bit
	test_opt<lulib::as_hex8_opt>			("       hex8: ",  "00");
	test_opt<lulib::as_hex8_opt>			("       hex8: ",  "7F");
	test_opt<lulib::as_hex8_opt>			("       hex8: ",  "80");
	test_opt<lulib::as_hex8_opt>			("       hex8: ",  "FF");
	test_opt<lulib::as_hex8_opt>			("       hex8: ", "100");
	// hex 16bit
	test_opt<lulib::as_hex16_opt>			("      hex16: ",  "0000");
	test_opt<lulib::as_hex16_opt>			("      hex16: ",  "7FFF");
	test_opt<lulib::as_hex16_opt>			("      hex16: ",  "8000");
	test_opt<lulib::as_hex16_opt>			("      hex16: ",  "FFFF");
	test_opt<lulib::as_hex16_opt>			("      hex16: ", "10000");
	// hex 32bit
	test_opt<lulib::as_hex32_opt>			("      hex32: ",  "00000000");
	test_opt<lulib::as_hex32_opt>			("      hex32: ",  "7FFFFFFF");
	test_opt<lulib::as_hex32_opt>			("      hex32: ",  "80000000");
	test_opt<lulib::as_hex32_opt>			("      hex32: ",  "FFFFFFFF");
	test_opt<lulib::as_hex32_opt>			("      hex32: ", "100000000");
	// hex 64bit
	test_opt<lulib::as_hex64_opt>			("      hex64: ",  "0000000000000000");
	test_opt<lulib::as_hex64_opt>			("      hex64: ",  "7FFFFFFFFFFFFFFF");
	test_opt<lulib::as_hex64_opt>			("      hex64: ",  "8000000000000000");
	test_opt<lulib::as_hex64_opt>			("      hex64: ",  "FFFFFFFFFFFFFFFF");
	test_opt<lulib::as_hex64_opt>			("      hex64: ", "10000000000000000");

}


int main() {
	test1();
	test2();

}

