#pragma once

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>

namespace lulib {

	template<typename F>
	class function_traits {
	public:
		typedef typename boost::function_types::result_type< F >::type result_type;
		typedef typename boost::function_types::parameter_types<F>::type parameter_types;
	};

}// namespace lulib
