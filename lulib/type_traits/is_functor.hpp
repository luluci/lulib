#pragma once

namespace lulib {

	template<typename T>
	class is_functor {
		template<typename U>
		struct aux {};

		typedef char yes;
		typedef struct { char foo[2]; } no;

		template<typename U>
		static yes test(aux< decltype(&U::operator()) >* = 0);
		template<typename U>
		static no test(...);

	public:
		static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
	};

}// namespace lulib

