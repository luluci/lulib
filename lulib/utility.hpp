#pragma once

#include <lulib/utility/enable_if.hpp>

namespace lulib {

	template<typename T>
	struct identity {
		typedef T type;
	};

	template < typename T >
	T && value();

}// namespace lulib
