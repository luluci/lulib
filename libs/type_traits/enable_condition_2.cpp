#include <iostream>
#include <type_traits>
#include <lulib/type_traits/enable_condition.hpp>

typedef lulib::enable_condition<
	std::is_same<lulib::_1, int>,
	std::is_integral<lulib::_1>,
	std::is_arithmetic<lulib::_1>
> enable;

template<typename T, typename Enabler = void> struct sfinae_test;

template<typename T>
struct sfinae_test<T, typename enable::case_<0,T,T>::type> {
	static const int value = 0;
};
template<typename T>
struct sfinae_test<T, typename enable::case_<1,T>::type> {
	static const int value = 1;
};
template<typename T>
struct sfinae_test<T, typename enable::case_<2,T>::type> {
	static const int value = 2;
};
template<typename T>
struct sfinae_test<T, typename enable::default_<T>::type> {
	static const int value = -1;
};

struct test {};

int main() {
	//std::cout << sfinae_test<int>::value << std::endl;  // duplication error: match is_same<> and is_integral<> and is_arithmetic<>
	//std::cout << sfinae_test<long>::value << std::endl;  // duplication error: match is_integral<> and is_arithmetic<>
	std::cout << sfinae_test<double>::value << std::endl;
	std::cout << sfinae_test<test>::value << std::endl;
}
