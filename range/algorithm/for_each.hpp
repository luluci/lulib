#pragma once

#include <lulib/range/adaptor/adaptor_traits.hpp>

#include <boost/utility/enable_if.hpp>

//#include <iostream>

namespace lulib {
	namespace range {

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			//std::cout << "true" << std::endl;
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::enable_if< is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			//std::cout << "true" << std::endl;
			return fun;
		}

		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range &rng, UnaryFunction const& fun) {
			//std::cout << "false" << std::endl;
			return fun;
		}
		template<typename Range, typename UnaryFunction>
		inline
		typename boost::disable_if< is_post_adaptor<Range>, UnaryFunction >::type
		for_each(Range const& rng, UnaryFunction const& fun) {
			auto it = boost::begin(rng), end = boost::end(rng);
			while (it != end) {
				it.apply(fun);
				++it;
			}
			return fun;
		}

	}//namespace lulib::range

}//namespace lulib
