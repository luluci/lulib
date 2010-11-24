#pragma once

#include <boost/mpl/push_back.hpp>

namespace lulib {
	namespace mpl{
		namespace detail {
			template<typename Sequence, typename Param, unsigned int n>
			class n_arg_push_back_impl {
				typedef typename boost::mpl::push_back<Sequence, Param>::type new_seq;
			public:
				typedef typename n_arg_push_back_impl<new_seq, Param, n-1>::type type;
			};
			template<typename Sequence, typename Param>
			class n_arg_push_back_impl<Sequence, Param, 0> {
			public:
				typedef Sequence type;
			};
		}//namespace detail

		// SequenceにN個のParam型をmpl::push_backする
		template<typename Sequence, typename Param, unsigned int n>
		struct n_arg_push_back {
			typedef typename detail::n_arg_push_back_impl<Sequence, Param, n>::type type;
		};

	}//namespace mpl
}//namespace lulib
