#pragma once

#include <boost/function_types/function_type.hpp>
#include <lulib/type_traits/mem_fn_traits.hpp>

namespace lulib {

	namespace detail {
		template<typename Functor>
		struct func_type {
			typedef decltype( &Functor::operator() ) type;
		};

	}// namespace detail

	template<typename Functor>
	class functor_traits {
		typedef typename detail::func_type<Functor>::type F;
		typedef lulib::mem_fn_traits<F> fn_traits;

	public:
		typedef typename fn_traits::result_type result_type;
		// boost::mpl::vector sequence
		typedef typename fn_traits::parameter_types parameter_types;
		// function type

	private:
		// result_type & parameter_types
		typedef typename boost::mpl::push_front<parameter_types, result_type>::type type_pack;

	public:
		typedef typename boost::function_types::function_type<type_pack>::type function_type;
	};

}// namespace lulib
