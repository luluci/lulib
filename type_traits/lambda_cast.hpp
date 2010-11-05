#pragma once

#include <functional>
#include <lulib/type_traits/functor_traits.hpp>

namespace lulib {

	namespace detail {
		template <typename Lambda>
		class lambda_to_function {
			typedef lulib::functor_traits<Lambda> lambda_traits;
		public:
			typedef std::function< typename lambda_traits::function_type > type;
		};
	}//namespace detail

	template<typename Lambda>
	inline
	auto lambda_cast(Lambda const& lambda) -> typename detail::lambda_to_function<Lambda>::type {
		return static_cast< typename detail::lambda_to_function<Lambda>::type >(lambda);
	}

}//namespace lulib
