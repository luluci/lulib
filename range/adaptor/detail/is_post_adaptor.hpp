#pragma once

#include <lulib/range/adaptor/adaptor_categories.hpp>
#include <lulib/range/adaptor/adaptor_traits.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/if.hpp>

namespace lulib { namespace range { namespace detail {

	template<typename Adaptor>
	struct is_post_adaptor {
		static const bool value = std::is_same<
			typename adaptor_traits<Adaptor>::adaptor_category,
			post_adaptor_tag
		>::value;
	};

}}}//namespace lulib::range::detail
