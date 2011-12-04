
#include <iostream>
#include <type_traits>

#include <lulib/type_traits/type_switch.hpp>
#include <lulib/type_info.hpp>

int main() {
	typedef lulib::type_switch<
		//lulib::type_case< std::is_arithmetic<boost::mpl::_1> ,char>,
		lulib::type_case< std::is_same<boost::mpl::_1, float> ,short>,
		lulib::type_case< std::is_same<boost::mpl::_1, double> ,long long>
	> type_switch;

	std::cout << lulib::type_info< type_switch::case_<int>::type >().name() << std::endl;
	std::cout << lulib::type_info< type_switch::case_<float>::type >().name() << std::endl;
	std::cout << lulib::type_info< type_switch::case_<double>::type >().name() << std::endl;

}
