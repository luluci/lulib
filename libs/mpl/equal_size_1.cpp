
#include <boost/mpl/placeholders.hpp>
#include <lulib/type_traits/equal_size.hpp>

typedef lulib::equal_size_c<boost::mpl::_1, 8>::type eq_type;

#include <iostream>
#include <lulib/type_info.hpp>

int main() {
	std::cout << lulib::type_info<eq_type>().name() << std::endl;
}

