#pragma once

namespace lulib {

	template<typename T>
	struct identity {
		typedef T type;
	};

	template < typename T >
	T && value();

	extern void * enabler;

}// namespace lulib
