#pragma once

#include <boost/optional.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace lulib { namespace program_option {

	template<typename T, typename Enable = void>
	struct value;

	// referenceの場合: コンストラクタは引数をとっても取らなくても
	template<typename T>
	struct value< T, typename boost::enable_if< boost::is_reference<T> >::type > {
		typedef T type;

		value(T t) : opt(t) {}

		boost::optional<T> opt;
	};
	// 非referenceの場合: コンストラクタは引数を取らない
	template<typename T>
	struct value< T, typename boost::disable_if< boost::is_reference<T> >::type > {
		typedef T type;

		value() {}

		boost::optional<T> opt;
	};

} } //namespace lulib::program_option
