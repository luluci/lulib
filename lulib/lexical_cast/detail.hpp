#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

#include <boost/spirit/include/qi.hpp>
/*
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
*/

#include <lulib/type_traits/is_nesting_type.hpp>
#include <lulib/type_traits/enable_condition.hpp>

#include <lulib/lexical_cast/qi_rule.hpp>
#include <lulib/lexical_cast/exception.hpp>

namespace lulib {
	namespace lexical_cast_detail {
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		// Asに対応した数値型を設定
		template<typename As, typename Enabler = void> struct attr;
		// 数値型ならそのまま
		template<typename As>
		struct attr<As, typename std::enable_if< std::is_arithmetic<As>::value >::type> {
			typedef As type;
		};
		// その他の型
		template<typename As>
		struct attr<As, typename std::enable_if< !std::is_arithmetic<As>::value >::type> {
			typedef typename As::value_type type;
		};

		// ネスト型としてresult_typeを持っているかをチェックするメタ関数を作成
		LULIB_GENERATE_IS_NESTING_TYPE(result_type);
		// Asに対応した返り値型を設定
		template<typename As, typename Enabler = void> struct result;
		// result_typeを持っている型なら
		template<typename As>
		struct result<As, typename std::enable_if< is_nesting_result_type<As>::value >::type> {
			typedef typename As::result_type type;
		};
		// result_typeを持っていないなら
		template<typename As>
		struct result<As, typename std::enable_if< !is_nesting_result_type<As>::value >::type> {
			typedef As type;
		};

		// rule enable_if
		typedef lulib::enable_condition<
			std::is_integral<lulib::_1>,
			std::is_floating_point<lulib::_1>,
			is_int_parser<lulib::_1>,
			is_real_parser<lulib::_1>,
			is_nesting_value_type<lulib::_1>
		> rule_enable;
		// Targetに対応したruleを作成
		template<typename As, typename Enabler = void> struct rule;
		// 整数型
		template<typename As>
		struct rule<As, typename rule_enable::if_<0, As>::type> {
			typedef typename int_parser<10, As>::type type;
		};
		// 浮動小数点型
		template<typename As>
		struct rule<As, typename rule_enable::else_if_<1, As>::type> {
			typedef typename real_parser<As>::type type;
		};
		// int_parser
		template<typename As>
		struct rule<As, typename rule_enable::else_if_<2, As>::type> {
			typedef typename As::type type;
		};
		// real_parser
		template<typename As>
		struct rule<As, typename rule_enable::else_if_<3, As>::type> {
			typedef typename As::type type;
		};
		// value_type
		template<typename As>
		struct rule<As, typename rule_enable::else_if_<4, As>::type> {
			typedef typename rule<typename As::value_type>::type type;
		};
		// other type
		template<typename As>
		struct rule<As, typename rule_enable::else_<As>::type> {
			typedef typename int_parser<10, As>::type type;
		};


		// qi grammar
		// 数値解析パーザ
		template<
			typename As, typename Iterator,
			typename Attr   = typename attr<As>::type,
			typename Parser = typename rule<As>::type
		>
		struct rule_traits : qi::grammar<Iterator, Attr(), qi::unused_type> {
			// ctor
			rule_traits() : rule_traits::base_type(rule_) {
				//rule_ %= Parser() >> -qi::lit_type()('\0') >> qi::eoi_type();
				rule_ %= Parser() >> -qi::lit('\0') >> qi::eoi;
			}

		private:
			qi::rule<Iterator, Attr(), qi::unused_type> rule_;
		};

		// 数値型を返す
		// 変換失敗時には例外を投げる
		template<typename As, typename Iterator>
		typename result<As>::type
		cast_impl(
			Iterator it, Iterator end,
			typename std::enable_if<!is_nesting_value_type<typename result<As>::type>::value>::type* = 0
		) {
			typedef typename result<As>::type result_type;
			static  rule_traits<As,Iterator> rule;
			static  qi::blank_type skipper;
			result_type result;
			bool ret = boost::spirit::qi::phrase_parse(it, end, rule, skipper, result);
			if (!ret) throw bad_lexical_cast(it, end, typeid(result_type));
			return result;
		}
		// ユーザ定義型を返す
		// エラーハンドリングはresult_typeに任せる
		template<typename As, typename Iterator>
		typename result<As>::type
		cast_impl(
			Iterator it, Iterator end,
			typename std::enable_if<is_nesting_value_type<typename result<As>::type>::value>::type* = 0
		) {
			typedef typename result<As>::type result_type;
			typedef typename value<result_type>::type value_type;
			static  rule_traits<As,Iterator> rule;
			static  qi::blank_type skipper;
			value_type value;
			bool ret = boost::spirit::qi::phrase_parse(it, end, rule, skipper, value);
			if (!ret) return result_type();
			return result_type(value);
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


