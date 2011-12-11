#pragma once

#include <boost/mpl/size_t.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/sizeof.hpp>

namespace lulib { namespace mpl {

	template<std::size_t Size>
	struct size : public boost::mpl::size_t<Size> {

		template<typename T>
		struct equal_to_sizeof : public boost::mpl::equal_to<
			boost::mpl::sizeof_<T>, boost::mpl::size_t<Size>
		> {};

	};

}}// namespace lulib::mpl

