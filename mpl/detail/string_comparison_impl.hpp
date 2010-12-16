#pragma once

#include <boost/type_traits.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/deref.hpp>

#include <boost/mpl/less.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/greater_equal.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/not_equal_to.hpp>

namespace lulib { namespace mpl { namespace detail {

	struct less_type {};
	struct greater_type {};
	struct equal_to_type {};

	template<typename Range1, bool Valid1, typename Range2, bool Valid2>
	struct range_comp_impl {};

	template<typename Begin, typename End>
	struct range {
		// begin/end
		typedef Begin begin;
		typedef End   end;
		// is end ?
		static const bool value = boost::is_same<begin,end>::value;
	};

	template<typename Range1, typename Range2>
	class next_and_comp {
		typedef range<
			typename boost::mpl::next< typename Range1::begin >::type,
			typename Range1::end
		> range1;
		typedef range<
			typename boost::mpl::next< typename Range2::begin >::type,
			typename Range2::end
		> range2;
	public:
		typedef typename range_comp_impl<range1, range1::value, range2, range2::value>::type type;
	};

	// Range1: begin == end
	// Range2: begin == end
	// range1 == range2
	template<typename Range1, typename Range2>
	struct range_comp_impl<Range1, true, Range2, true> { typedef equal_to_type type; };
	// Range1: begin != end
	// Range2: begin == end
	// range1 > range2
	template<typename Range1, typename Range2>
	struct range_comp_impl<Range1, false, Range2, true> { typedef greater_type type; };
	// Range1: begin == end
	// Range2: begin != end
	// range1 < range2
	template<typename Range1, typename Range2>
	struct range_comp_impl<Range1, true, Range2, false> { typedef less_type type; };
	// Range1: begin != end
	// Range2: begin != end
	template<typename Range1, typename Range2>
	struct range_comp_impl<Range1, false, Range2, false> {
		// 比較
		typedef typename boost::mpl::if_<
			// less?
			typename boost::mpl::less<
				typename boost::mpl::deref<typename Range1::begin>::type,
				typename boost::mpl::deref<typename Range2::begin>::type
			>::type,
			// Range1::begin < Range2::begin
			less_type,
			// Range1::begin >= Range2::begin
			typename boost::mpl::if_<
				// greater?
				typename boost::mpl::greater<
					typename boost::mpl::deref<typename Range1::begin>::type,
					typename boost::mpl::deref<typename Range2::begin>::type
				>::type,
				// Range1::begin > Range2::begin
				greater_type,
				// Range1::begin == Range2::begin
				typename next_and_comp<Range1, Range2>::type
			>::type
		>::type type;
	};

	template<typename Str1, typename Str2>
	class string_comp_impl {
		typedef range< typename boost::mpl::begin<Str1>::type, typename boost::mpl::end<Str1>::type > range1;
		typedef range< typename boost::mpl::begin<Str2>::type, typename boost::mpl::end<Str2>::type > range2;
	public:
		typedef typename range_comp_impl<range1, range1::value, range2, range2::value>::type type;
	};

	template<typename Str1, typename Str2>
	class string_equal_to_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = boost::is_same< result, equal_to_type >::value;
	};
	template<typename Str1, typename Str2>
	class string_not_equal_to_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = ! boost::is_same< result, equal_to_type >::value;
	};
	template<typename Str1, typename Str2>
	class string_less_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = boost::is_same< result, less_type >::value;
	};
	template<typename Str1, typename Str2>
	class string_less_equal_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = boost::is_same< result, less_type >::value | boost::is_same< result, equal_to_type >::value;
	};
	template<typename Str1, typename Str2>
	class string_greater_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = boost::is_same< result, greater_type >::value;
	};
	template<typename Str1, typename Str2>
	class string_greater_equal_impl {
		typedef typename string_comp_impl<Str1, Str2>::type result;
	public:
		static const bool value = boost::is_same< result, greater_type >::value | boost::is_same< result, equal_to_type >::value;
	};

} } }// namespace lulib::mpl::detail
