#pragma once

#include <lulib/mpl/detail/string_comparison_impl.hpp>

namespace lulib { namespace mpl {

	template<typename Str1, typename Str2>
	struct string_greater_equal {
		typedef detail::string_greater_equal_impl<Str1, Str2> type;
	};

} } // namespace lulib::mpl
