#pragma once

#include <boost/mpl/pop_front.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>

namespace lulib {

	template<typename F>
	class mem_fn_traits {
		// boost::mpl::vector sequence
		// メンバー関数の場合、vector[0]が自身への参照
		typedef typename boost::function_types::parameter_types<F>::type selfref_and_parameter;

	public:
		//
		typedef typename boost::function_types::result_type< F >::type result_type;
		// boost::mpl::vector sequence
		typedef typename boost::mpl::pop_front<selfref_and_parameter>::type parameter_types;
	};

}// namespace lulib
