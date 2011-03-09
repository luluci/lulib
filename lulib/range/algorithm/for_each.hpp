#pragma once

#include <lulib/range/detail/is_post_range.hpp>

#include <boost/range/algorithm/for_each.hpp>
#include <boost/utility/enable_if.hpp>

//#include <iostream>

namespace lulib {
	namespace range {

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< detail::is_post_range<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			//std::cout << "post_range &" << std::endl;
			auto it = boost::begin(rng), end = boost::end(rng);
			while (it != end) {
				it.apply(fun);
				++it;
			}
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< detail::is_post_range<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			//std::cout << "post_range const&" << std::endl;
			auto it = boost::begin(rng), end = boost::end(rng);
			while (it != end) {
				it.apply(fun);
				++it;
			}
			return fun;
		}

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< detail::is_post_range<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			//std::cout << "range &" << std::endl;
			boost::for_each(rng, fun);
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< detail::is_post_range<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			//std::cout << "range &" << std::endl;
			boost::for_each(rng, fun);
			return fun;
		}

	}//namespace lulib::range

}//namespace lulib
