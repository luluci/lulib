
#include <lulib/type_traits/is_nesting_type.hpp>

namespace hoge {
	LULIB_GENERATE_IS_NESTING_TYPE(value_type)
}
namespace huga {
	LULIB_GENERATE_IS_NESTING_TYPE(value)
}

struct test1 {
	typedef int value_type;
};
struct test2 {
	typedef void value_type;
};
struct test3 {
	typedef char value;
};

#include <iostream>

template<typename T>
void func() {
	std::cout << hoge::is_nesting_value_type<T>::value << std::endl;
}
template<typename T>
void func2() {
	std::cout << huga::is_nesting_value<T>::value << std::endl;
}

int main() {
	std::cout << std::boolalpha;
	func<test1>();
	func<test2>();
	func<test3>();
	func2<test3>();
}
