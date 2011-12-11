
#include <iostream>
#include <type_traits>

#include <boost/mpl/int.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/lambda.hpp>

#include <boost/mpl/size_t.hpp>

#include <lulib/mpl/size.hpp>
#include <lulib/mpl/equal_size.hpp>

#include <lulib/type_traits/type_switch.hpp>
#include <lulib/type_info.hpp>

int main() {
	typedef lulib::type_switch<
		/*
		lulib::type_case< boost::mpl::equal_to< boost::mpl::sizeof_<boost::mpl::_1>, boost::mpl::int_<4> >, int >,
		lulib::type_case< boost::mpl::equal_to< boost::mpl::sizeof_<boost::mpl::_1>, boost::mpl::int_<8> >, long long >
		*/
		lulib::type_case< lulib::equal_size<boost::mpl::_1, int>, int >,
		lulib::type_case< lulib::equal_size<boost::mpl::_1, long long >, long long >,
		lulib::type_case< lulib::is_equal_size<boost::mpl::_1, boost::mpl::size_t<1> >, char >,
		lulib::type_case< lulib::mpl::size<2>::equal_to_sizeof<boost::mpl::_1>, short >
	> type_switch;

	std::cout << lulib::type_info< type_switch::case_<int>::type >().name() << std::endl;
	std::cout << lulib::type_info< type_switch::case_<long long>::type >().name() << std::endl;
	std::cout << lulib::type_info< type_switch::case_<char>::type >().name() << std::endl;
	std::cout << lulib::type_info< type_switch::case_<short>::type >().name() << std::endl;

}
