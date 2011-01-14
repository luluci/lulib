#pragma once

namespace lulib {
	namespace type_traits {

		template<typename T>
		struct has_push_back {
			template<typename U, void(U::*)(const typename U::value_type&)>
			struct has_func {};

			typedef char yes;
			typedef struct { char foo[2]; } no;

			template<typename U>
			static yes test(U*, has_func<U, &U::push_back>* = 0);
			template<typename U>
			static no test(...);

		public:
			static const bool value = (sizeof(test<T>(0)) == sizeof(yes));
		};

	}// namespace type_traits
}// namespace lulib
