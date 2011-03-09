#pragma once

#include <functional>

#include <lulib/mpl/n_arg_push_back.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/function_types/function_type.hpp>

namespace lulib {
	namespace function_types {
		namespace detail {
			template<typename Result, typename Param, unsigned int n>
			class n_arg_function_impl {
				typedef typename lulib::mpl::n_arg_push_back< boost::mpl::vector<Result>, Param, n >::type params;
				typedef typename boost::function_types::function_type<params>::type signature;
			public:
				typedef std::function<signature> type;
			};
		}

		// N個のParam型を引数にとるstd::function<>を作成
		template<typename Result, typename Param, unsigned int n>
		struct n_arg_function {
			typedef typename detail::n_arg_function_impl<Result, Param, n>::type type;
		};

	}//namespace function_types
}//namespace lulib
