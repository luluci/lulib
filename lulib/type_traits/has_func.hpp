#pragma once

#include <type_traits>  // std::true_type, std::enable_if

#include <lulib/utility.hpp>  // lulib::identity, lulib::value

#define LULIB_GEN_HAS_FUNC(func) \
namespace lulib { \
	namespace type_traits { \
		template<typename T> \
		struct has_##func { \
		private: \
			template<typename U> \
			static auto check(U &&u) -> decltype( u.func(), std::true_type() ); \
			static std::false_type check(...); \
		public: \
			static bool const value = lulib::identity< decltype( check( value<T>() ) ) >::type::value; \
		}; \
	} \
} \

