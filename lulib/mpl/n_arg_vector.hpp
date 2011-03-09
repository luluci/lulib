#pragma once

#include <lulib/mpl/n_arg_push_back.hpp>

#include <boost/mpl/vector.hpp>

namespace lulib {
	namespace mpl{
		// N個のParam型をもつmpl::vectorを作成する
		template<typename Param, unsigned int n>
		struct n_arg_vector {
			typedef typename lulib::mpl::n_arg_push_back< boost::mpl::vector<>, Param, n >::type type;
		};

	}//namespace mpl
}//namespace lulib
