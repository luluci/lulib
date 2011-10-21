#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

#include <lulib/lexical_cast/qi_rule.hpp>

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
//#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

namespace lulib {
	namespace lexical_cast_detail {
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		// qi grammar
		// 数値解析パーザ
		template<
			typename As, typename Iterator,
			typename Result = typename result<As>::type,
			typename Parser = typename rule<As>::type
		>
		struct rule_traits : qi::grammar<Iterator, Result(), ascii::space_type> {
			// ctor
			rule_traits() : rule_traits::base_type(rule_) {
				rule_ %= Parser() >> -qi::lit_type()('\0') >> qi::eoi_type();
			}

		private:
			qi::rule<Iterator, Result(), ascii::space_type> rule_;
		};

		// by iterator
		template<typename As, typename Iterator>
		typename result<As>::type
		cast_impl(Iterator it, Iterator end) {
			typedef typename result<As>::type result_type;
			static  rule_traits<As,Iterator> rule;
			static  ascii::space_type space;
			result_type result;
			bool ret = boost::spirit::qi::phrase_parse(it, end, rule, space, result);
			if (!ret) return result_type();
			return result;
		}
		/*
		// by range
		template<typename As, typename Range>
		typename result<As>::type
		cast_impl(Range &&range) {
			return cast_impl( std::begin(range), std::end(range) );
		}
		*/
	}//namespace lexical_cast_detail

}// namespace lulib


