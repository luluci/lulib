#pragma once
#pragma warning(disable : 4819)

#include <string>

#include <lulib/type_traits/is_instance_of.hpp>
#include <lulib/type_traits/is_nesting_type.hpp>
#include <lulib/type_traits/enable_condition.hpp>

namespace lulib {

	namespace char_traits_detail {
		using std::basic_string;
		LULIB_GENERATE_IS_INSTANCE_OF(
			(typename Char, typename Traits, typename Alloc),
			basic_string,
			(Char,Traits,Alloc)
		)
		LULIB_GENERATE_IS_NESTING_TYPE(value_type)

		typedef enable_condition<
			is_instance_of_basic_string<lulib::_1>,
			is_nesting_value_type<lulib::_1>,
			std::is_array<lulib::_1>,
			std::is_pointer<lulib::_1>
		> enable;
	}// namespace lulib::char_traits_detail

	// char 特性
	template<typename T, typename Enable = void>
	struct char_traits;

	// std::basic_stringクラス
	template<typename T>
	struct char_traits<T, typename char_traits_detail::enable::if_<0,T>::type> {
		typedef typename T::value_type char_type;
		typedef T                      string_type;
	};

	// T::value_type を持つクラス
	template<typename T>
	struct char_traits<T, typename char_traits_detail::enable::else_if_<1,T>::type> {
		typedef typename T::value_type       char_type;
		typedef std::basic_string<char_type> string_type;
	};

	// 配列
	template<typename T>
	struct char_traits<T, typename char_traits_detail::enable::else_if_<2,T>::type> {
		typedef typename std::remove_pointer<
			typename std::remove_cv<
				typename std::decay<T>::type
			>::type
		>::type char_type;
		typedef std::basic_string<char_type> string_type;
	};

	// ポインタ
	template<typename T>
	struct char_traits<T, typename char_traits_detail::enable::else_if_<3,T>::type> {
		typedef typename std::remove_pointer<
			typename std::remove_cv<T>::type
		>::type char_type;
		typedef std::basic_string<char_type> string_type;
	};

	// その他
	template<typename T>
	struct char_traits<T, typename char_traits_detail::enable::else_<T>::type> {
		typedef T                            char_type;
		typedef std::basic_string<char_type> string_type;
	};

}// namespace lulib
