#pragma once

#include <lulib/range/range_categories.hpp>
#include <lulib/range/range_traits.hpp>

namespace lulib { namespace range { namespace detail {

	template<typename Range>
	struct is_post_range {
		static const bool value = std::is_same<
			typename range_traits<Range>::range_category,
			range::post_range_tag
		>::value;
	};

}}}//namespace lulib::range::detail
