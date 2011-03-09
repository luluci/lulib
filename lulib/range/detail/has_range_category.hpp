#pragma once

#include <lulib/range/range_categories.hpp>

namespace lulib { namespace range { namespace detail {

	// Rangeがrange_categoryを持っているか
	template<typename Range>
	class has_range_category {
		// adaptor_categoryタグを持ってる
		template<typename R>
		static std::true_type test(typename R::range_category* = 0);
		// 持ってない
		template<typename R>
		static std::false_type test(...);

	public:
		static const bool value = std::is_same< decltype(test<Range>(0)), std::true_type>::value;
	};

}}}//namespace lulib::range::detail
