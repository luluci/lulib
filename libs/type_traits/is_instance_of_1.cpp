
#include <lulib/type_traits/is_instance_of.hpp>

template<typename T, typename U>
struct test1 {};
template<typename T, std::size_t S, typename U>
struct test2 {};


namespace hoge {
	LULIB_GENERATE_IS_INSTANCE_OF(
		(typename X, typename Y),
		test1,
		(X,Y)
	)
}
namespace huga {
	LULIB_GENERATE_IS_INSTANCE_OF(
		(typename X, std::size_t S, typename Y),
		test2,
		(X,S,Y)
	)
}

#include <iostream>

template<typename T>
void func() {
	std::cout << hoge::is_instance_of_test1<T>::value << std::endl;
}
template<typename T>
void func2() {
	std::cout << huga::is_instance_of_test2<T>::value << std::endl;
}

int main() {
	std::cout << std::boolalpha;
	func< test1<int,char>    >();
	func< test2<int,2,short> >();
	func2< test1<int,char>    >();
	func2< test2<int,2,short> >();
}
