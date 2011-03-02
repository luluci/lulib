#pragma once

#include <lulib/range/adaptor/adaptor_categories.hpp>
//#include <lulib/range/adaptor/detail/has_adaptor_category.hpp>

//#include <boost/utility/enable_if.hpp>
//#include <boost/mpl/if.hpp>

namespace lulib { namespace range {

	// detail
	namespace detail {
		template<typename Adaptor>
		struct adaptor_category {
			// adaptor_categoryタグを持ってる
			template<typename U>
			static typename U::adaptor_category test(typename U::adaptor_category* = 0);
			// 持ってない
			template<typename U>
			static adaptor_tag test(...);

		public:
			typedef decltype(test<Adaptor>(0)) type;
		};
	}

	template<typename Adaptor>
	struct adaptor_traits {
		typedef typename detail::adaptor_category<Adaptor>::type adaptor_category;
		typedef typename Adaptor::iterator iterator;
	};

}}//namespace lulib::range
