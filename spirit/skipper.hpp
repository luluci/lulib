#pragma once

#include <boost/spirit/home/support/char_class.hpp>
#include <boost/proto/proto.hpp>

namespace lulib { namespace spirit {

	namespace detail {
		template<typename SkipChar>
		struct skipper_impl {
			typedef char char_type;
			static bool ischar(int ch) {
				return SkipChar::eval(ch);
			}
		};
	}

	template<typename SkipChar>
	struct skipper {
		typedef typename boost::proto::terminal<
			boost::spirit::tag::char_code<
				boost::spirit::tag::char_,
				detail::skipper_impl<SkipChar>
			>
		>::type type;
	};

} }// namespace lulib::spirit



/*
// qi::ascii::space
qi::ascii::space_type const space;  // というインスタンス

// qi::ascii::space_type type
== boost::proto::exprns_::expr<
	boost::proto::tag::terminal,
	boost::proto::argsns_::term<
		boost::spirit::tag::char_code<
			boost::spirit::tag::space,
			boost::spirit::char_encoding::ascii>
		>,
	0l
>
== 
typedef proto::terminal<tag::char_code<tag::space, spirit::char_encoding::ascii> >::type space_type;
*/

