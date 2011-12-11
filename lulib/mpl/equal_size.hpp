#pragma once

#include <boost/mpl/size_t.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/sizeof.hpp>

namespace lulib {

	// sizeof(T) == U::value
	template<typename T, typename U>
	struct is_equal_size : public boost::mpl::equal_to<
		boost::mpl::sizeof_<T>, U
	> {};

	// sizeof(T) == Size
	template<typename T, std::size_t Size>
	struct is_equal_size_c : public boost::mpl::equal_to<
		boost::mpl::sizeof_<T>, boost::mpl::size_t<Size>
	> {};

	// sizeof(T) == sizeof(U)
	template<typename T, typename U>
	struct equal_size : public boost::mpl::equal_to<
		boost::mpl::sizeof_<T>, boost::mpl::sizeof_<U>
	> {};

	// sizeof(T) == size
	template<typename T, std::size_t Size>
	struct equal_size_c : public boost::mpl::equal_to<
		boost::mpl::sizeof_<T>, boost::mpl::size_t<Size>
	> {};

}// namespace lulib

