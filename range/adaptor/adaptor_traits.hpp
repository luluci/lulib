#pragma once

#include <lulib/range/adaptor/adaptor_categories.hpp>

#include <boost/utility/enable_if.hpp>
#include <boost/mpl/if.hpp>

namespace lulib { namespace range {

	template<typename T>
	class has_adaptor_category {
		// adaptor_categoryタグを持ってる
		template<typename U>
		static std::true_type test(typename U::adaptor_category* = 0);
		// 持ってない
		template<typename U>
		static std::false_type test(...);

	public:
		static const bool value = std::is_same< decltype(test<T>(0)), std::true_type>::value;
	};

	template<typename T>
	class is_post_adaptor {

		template<typename U, typename Enable = void>
		struct check;

		template<typename U>
		struct check<U, typename boost::enable_if< has_adaptor_category<U> >::type>
		{
			typedef typename U::adaptor_category adaptor_category;
		};

		template<typename U>
		struct check<U, typename boost::disable_if< has_adaptor_category<U> >::type>
		{
			typedef adaptor_tag adaptor_category;
		};

	public:
		static const bool value = std::is_same< typename check<T>::adaptor_category, post_adaptor_tag>::value;
	};

}}//namespace lulib::range
