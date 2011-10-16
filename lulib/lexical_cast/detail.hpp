#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
//#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>

namespace lulib {
	namespace lexical_cast_detail {
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		namespace mpl = boost::mpl;

		// int parser wrapper
		// signed/unsigned 両方受け付ける
		template<
			std::size_t Radix,                // 何進数か
			typename Numeric = std::size_t,   // サイズ
			std::size_t MinDigits = 1,        // 最低何字の数字を取るか
			int MaxDigits = -1,               // 最大何字まで数字を取るか
			typename Enable = void
		> struct int_parser;
		// signed specialize
		template<std::size_t Radix, typename Numeric, std::size_t Min, int Max>
		struct int_parser<
			Radix,Numeric,Min,Max,
			typename std::enable_if<std::is_signed<Numeric>::value>::type
		> {
			typedef Numeric                                  result_type;
			typedef qi::int_parser<Numeric, Radix, Min, Max>        type;
		};
		// unsigned specialize
		template<std::size_t Radix, typename Numeric, std::size_t Min, int Max>
		struct int_parser<
			Radix,Numeric,Min,Max,
			typename std::enable_if<std::is_unsigned<Numeric>::value>::type
		> {
			typedef Numeric                                   result_type;
			typedef qi::uint_parser<Numeric, Radix, Min, Max>        type;
		};
		// floationg point specialize
		template<typename Numeric, typename Policy = qi::real_policies<Numeric>>
		struct real_parser {
			typedef Numeric                          result_type;
			typedef qi::real_parser<Numeric, Policy>        type;
		};

		// X進数ルール作成用
		template<std::size_t Radix, typename Numeric = std::size_t, std::size_t Min = 1, int Max = -1>
		struct notation {
			typedef Numeric                                             result_type;
			typedef typename int_parser<Radix, Numeric, Min, Max>::type        type;
		};
		// notation検出用メタクラス
		template<typename T>
		class is_notation {
			template<std::size_t R, typename N, std::size_t Mi, int Ma>
			static std::true_type test(notation<R,N,Mi,Ma> &&);
			static std::false_type test(...);
		public:
			//static const bool value = decltype( test(T()) )::value;
			static const bool value = std::is_same< decltype(test(T())), std::true_type >::value;
		};

		// Targetに対応した返り値型を得る
		template<typename Target, typename Enabler = void> struct result;
		// 数値型ならそのまま
		template<typename Target>
		struct result<Target, typename std::enable_if< std::is_arithmetic<Target>::value >::type> {
			typedef Target type;
		};
		// その他の型
		template<typename Target>
		struct result<Target, typename std::enable_if< !std::is_arithmetic<Target>::value >::type> {
			typedef typename Target::result_type type;
		};

		// Targetに対応したruleを作成
		template<typename Target, typename Enabler = void> struct rule;
		// 整数型
		template<typename Target>
		struct rule<Target, typename std::enable_if< std::is_integral<Target>::value >::type> {
			typedef Target                            result_type;
			typedef typename int_parser<10, Target>::type        type;
		};
		// 浮動小数点型
		template<typename Target>
		struct rule<Target, typename std::enable_if< std::is_floating_point<Target>::value >::type> {
			typedef Target                             result_type;
			typedef typename real_parser<Target>::type        type;
		};
		// notationなど
		template<typename Target>
		struct rule<Target, typename std::enable_if< is_notation<Target>::value >::type> {
			typedef typename Target::result_type result_type;
			typedef typename Target::type               type;
		};

		// qi grammar
		// 数値解析パーザ
		template<
			typename Target, typename Iterator,
			typename Result = typename result<Target>::type,
			typename Parser = typename rule<Target>::type
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
		template<typename Target, typename Iterator>
		typename result<Target>::type
		cast_impl(Iterator it, Iterator end) {
			typedef typename result<Target>::type result_type;
			static  rule_traits<Target,Iterator> rule;
			static  ascii::space_type space;
			result_type result;
			bool ret = boost::spirit::qi::phrase_parse(it, end, rule, space, result);
			if (!ret) return result_type();
			return result;
		}
		/*
		// by range
		template<typename Target, typename Range>
		typename result<Target>::type
		cast_impl(Range &&range) {
			return cast_impl( std::begin(range), std::end(range) );
		}
		*/
	}//namespace lexical_cast_detail

	// XX進数解析ルール
	// 自動で違う進数とみなしたりはしない
	// 16進数
	typedef lexical_cast_detail::notation<16,  std::uint8_t, 1, -1> hex8_t;
	typedef lexical_cast_detail::notation<16, std::uint16_t, 1, -1> hex16_t;
	typedef lexical_cast_detail::notation<16, std::uint32_t, 1, -1> hex32_t;
	typedef lexical_cast_detail::notation<16, std::uint64_t, 1, -1> hex64_t;
	typedef hex32_t hex;
	// 8進数
	typedef lexical_cast_detail::notation<8,  std::uint8_t, 1, -1> oct8_t;
	typedef lexical_cast_detail::notation<8, std::uint16_t, 1, -1> oct16_t;
	typedef lexical_cast_detail::notation<8, std::uint32_t, 1, -1> oct32_t;
	typedef lexical_cast_detail::notation<8, std::uint64_t, 1, -1> oct64_t;
	typedef oct32_t oct;
	// 2進数
	typedef lexical_cast_detail::notation<2,  std::uint8_t, 1, -1> bin8_t;
	typedef lexical_cast_detail::notation<2, std::uint16_t, 1, -1> bin16_t;
	typedef lexical_cast_detail::notation<2, std::uint32_t, 1, -1> bin32_t;
	typedef lexical_cast_detail::notation<2, std::uint64_t, 1, -1> bin64_t;
	typedef bin32_t bin;

}// namespace lulib


