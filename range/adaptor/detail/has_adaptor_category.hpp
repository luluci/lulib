#pragma once

#include <lulib/range/adaptor/adaptor_categories.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/if.hpp>

namespace lulib { namespace range {

	template<typename T>
	class has_adaptor_category {
		// adaptor_categoryタグを持ってる
		template<typename U>
		static std::true_type test(typename U::adaptor_category* = 0);
		// 持ってない
		template<typename U>
		static std::false_type test(...);

	public:
		static const bool value = std::is_same< decltype(test<T>(0)), std::true_type>::value;
	};

}}//namespace lulib::range
