#pragma once

#include <lulib/range/range_categories.hpp>

namespace lulib {

	// detail
	namespace detail {
		template<typename Range>
		struct range_category {
			// adaptor_categoryタグを持ってる
			template<typename R>
			static typename R::range_category test(typename R::range_category* = 0);
			// 持ってない
			template<typename R>
			static range::default_range_tag test(...);
		public:
			typedef decltype(test<Range>(0)) type;
		};
	}

	template<typename Range>
	struct range_traits {
		typedef typename detail::range_category<Range>::type range_category;

		typedef typename Range::iterator iterator;
		typedef typename iterator::iterator_category iterator_category;
	};

}//namespace lulib
