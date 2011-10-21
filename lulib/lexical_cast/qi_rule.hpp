#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#include <boost/spirit/include/qi_numeric.hpp>
#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

namespace lulib {
	namespace lexical_cast_detail {
		namespace qi = boost::spirit::qi;

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
		template<typename As, typename Enabler = void> struct result;
		// 数値型ならそのまま
		template<typename As>
		struct result<As, typename std::enable_if< std::is_arithmetic<As>::value >::type> {
			typedef As type;
		};
		// その他の型
		template<typename As>
		struct result<As, typename std::enable_if< !std::is_arithmetic<As>::value >::type> {
			typedef typename As::result_type type;
		};

		// Targetに対応したruleを作成
		template<typename As, typename Enabler = void> struct rule;
		// 整数型
		template<typename As>
		struct rule<As, typename std::enable_if< std::is_integral<As>::value >::type> {
			typedef As                                result_type;
			typedef typename int_parser<10, As>::type        type;
		};
		// 浮動小数点型
		template<typename As>
		struct rule<As, typename std::enable_if< std::is_floating_point<As>::value >::type> {
			typedef As                             result_type;
			typedef typename real_parser<As>::type        type;
		};
		// notationなど
		template<typename As>
		struct rule<As, typename std::enable_if< is_notation<As>::value >::type> {
			typedef typename As::result_type result_type;
			typedef typename As::type               type;
		};

	}//namespace lexical_cast_detail

	// XX進数解析ルール
	// 自動で違う進数とみなしたりはしない
	// 16進数
	typedef lexical_cast_detail::notation<16,  std::uint8_t, 1, -1> as_hex8;
	typedef lexical_cast_detail::notation<16, std::uint16_t, 1, -1> as_hex16;
	typedef lexical_cast_detail::notation<16, std::uint32_t, 1, -1> as_hex32;
	typedef lexical_cast_detail::notation<16, std::uint64_t, 1, -1> as_hex64;
	typedef as_hex32 as_hex;
	// 8進数
	typedef lexical_cast_detail::notation<8,  std::uint8_t, 1, -1> as_oct8;
	typedef lexical_cast_detail::notation<8, std::uint16_t, 1, -1> as_oct16;
	typedef lexical_cast_detail::notation<8, std::uint32_t, 1, -1> as_oct32;
	typedef lexical_cast_detail::notation<8, std::uint64_t, 1, -1> as_oct64;
	typedef as_oct32 as_oct;
	// 2進数
	typedef lexical_cast_detail::notation<2,  std::uint8_t, 1, -1> as_bin8;
	typedef lexical_cast_detail::notation<2, std::uint16_t, 1, -1> as_bin16;
	typedef lexical_cast_detail::notation<2, std::uint32_t, 1, -1> as_bin32;
	typedef lexical_cast_detail::notation<2, std::uint64_t, 1, -1> as_bin64;
	typedef as_bin32 as_bin;

	// 10進数
	typedef lexical_cast_detail::int_parser<10,   std::int8_t, 1, -1>   as_int8;
	typedef lexical_cast_detail::int_parser<10,  std::uint8_t, 1, -1>  as_uint8;
	typedef lexical_cast_detail::int_parser<10,  std::int16_t, 1, -1>  as_int16;
	typedef lexical_cast_detail::int_parser<10, std::uint16_t, 1, -1> as_uint16;
	typedef lexical_cast_detail::int_parser<10,  std::int32_t, 1, -1>  as_int32;
	typedef lexical_cast_detail::int_parser<10, std::uint32_t, 1, -1> as_uint32;
	typedef lexical_cast_detail::int_parser<10,  std::int64_t, 1, -1>  as_int64;
	typedef lexical_cast_detail::int_parser<10, std::uint64_t, 1, -1> as_uint64;
	typedef as_uint32 as_uint;
	typedef as_int32   as_int;

}// namespace lulib


