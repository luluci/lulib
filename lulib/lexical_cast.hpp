#pragma once
#pragma warning(disable : 4819)

#include <lulib/lexical_cast/detail.hpp>

namespace lulib {
	template<typename Target, typename Iterator>
	//typename lexical_cast_detail::result<Target>::type
	auto lexical_cast(Iterator it, Iterator end)
	-> decltype(lexical_cast_detail::cast_impl<Target>(it, end))
	{
		return lexical_cast_detail::cast_impl<Target>(it, end);
	}
	template<typename Target, typename Range>
	//typename lexical_cast_detail::result<Target>::type
	auto lexical_cast(Range &&range)
	-> decltype(lexical_cast_detail::cast_impl<Target>(std::begin(range), std::end(range)))
	{
		return lexical_cast_detail::cast_impl<Target>(std::begin(range), std::end(range));
	}
	/*
	template<typename Target, typename T, std::size_t N>
	typename lexical_cast_detail::result<Target>::type
	//decltype(lexical_cast_detail::cast_impl<Target>(std::begin(range), std::end(range)))
	lexical_cast(T (&array)[N]) {
		return lexical_cast_detail::cast_impl<Target>(std::begin(array), std::end(array));
	}
	*/
}// namespace lulib

