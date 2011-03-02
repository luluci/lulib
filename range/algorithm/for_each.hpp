#pragma once

#include <lulib/range/adaptor/detail/is_post_adaptor.hpp>

#include <boost/utility/enable_if.hpp>

#include <iostream>

namespace lulib {
	namespace range {

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< detail::is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			std::cout << "post_adaptor &" << std::endl;
			auto it = boost::begin(rng), end = boost::end(rng);
			while (it != end) {
				it.apply(fun);
				++it;
			}
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< detail::is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			std::cout << "post_adaptor const&" << std::endl;
			auto it = boost::begin(rng), end = boost::end(rng);
			while (it != end) {
				it.apply(fun);
				++it;
			}
			return fun;
		}

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< detail::is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			std::cout << "adaptor &" << std::endl;
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< detail::is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			std::cout << "adaptor &" << std::endl;
			return fun;
		}

	}//namespace lulib::range

}//namespace lulib
