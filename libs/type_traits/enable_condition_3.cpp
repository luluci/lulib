#include <iostream>
#include <type_traits>
#include <lulib/type_traits/enable_condition.hpp>

typedef lulib::disable_condition<
	std::is_arithmetic<lulib::_1>,
	std::is_integral<lulib::_1>,
	std::is_same<lulib::_1, int>
> disable;

template<typename T, typename Enabler = void> struct sfinae_test;

template<typename T>
struct sfinae_test<T, typename disable::if_<0,T,T>::type> {
	static const int value = 0;
};
template<typename T>
struct sfinae_test<T, typename disable::else_if_<1,T>::type> {
	static const int value = 1;
};
template<typename T>
struct sfinae_test<T, typename disable::else_if_<2,T>::type> {
	static const int value = 2;
};
template<typename T>
struct sfinae_test<T, typename disable::else_<T>::type> {
	static const int value = -1;
};

struct test {};

int main() {
	std::cout << sfinae_test<test>::value << std::endl;
	std::cout << sfinae_test<float>::value << std::endl;
	std::cout << sfinae_test<long>::value << std::endl;
	std::cout << sfinae_test<int>::value << std::endl;
}
