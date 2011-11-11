#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

//#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#include <boost/spirit/include/qi_numeric.hpp>
//#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS

#include <lulib/type_traits/is_nesting_type.hpp>

namespace lulib {
	namespace lexical_cast_detail {
		namespace qi = boost::spirit::qi;

		// ネスト型としてvalue_typeを持っているかをチェックするメタ関数を作成
		LULIB_IS_NESTING_TYPE(value_type);

		// Resultに対応した数値型を設定
		template<typename Result, typename Enabler = void> struct value;
		// 数値の場合
		template<typename Result>
		struct value<Result, typename std::enable_if< std::is_arithmetic<Result>::value >::type> {
			typedef Result type;
		};
		// value_typeを持つクラスの場合
		template<typename Result>
		struct value<Result, typename std::enable_if< is_nesting_value_type<Result>::value >::type> {
			typedef typename Result::value_type type;
		};

		// int parser wrapper
		// signed/unsigned 両方受け付ける
		template<
			std::size_t Radix,                              // 何進数か
			typename Result = std::size_t,                  // 返り値型
			std::size_t MinDigits = 1,                      // 最低何字の数字を取るか
			int MaxDigits = -1,                             // 最大何字まで数字を取るか
			typename Value = typename value<Result>::type,  // 数値型
			typename Enable = void
		> struct int_parser;
		// signed specialize
		template<std::size_t Radix, typename Result, std::size_t Min, int Max, typename Value>
		struct int_parser<
			Radix, Result, Min, Max, Value,
			typename std::enable_if<std::is_signed<Value>::value>::type
		> {
			typedef Result  result_type;  // 返り値型
			typedef Value   value_type;   // 数値型
			typedef qi::int_parser<value_type, Radix, Min, Max> type;  // qi rule型
		};
		// unsigned specialize
		template<std::size_t Radix, typename Result, std::size_t Min, int Max, typename Value>
		struct int_parser<
			Radix, Result, Min, Max, Value,
			typename std::enable_if<std::is_unsigned<Value>::value>::type
		> {
			typedef Result  result_type;  // 返り値型
			typedef Value   value_type;   // 数値型
			typedef qi::uint_parser<value_type, Radix, Min, Max> type;  // qi rule型
		};
		// int_parser検出用メタクラス
		template<typename T>
		class is_int_parser {
			template<std::size_t R, typename N, std::size_t Mi, int Ma, typename V>
			static std::true_type test(int_parser<R,N,Mi,Ma,V> &&);
			static std::false_type test(...);
		public:
			//static const bool value = decltype( test(T()) )::value;
			static const bool value = std::is_same< decltype(test(T())), std::true_type >::value;
		};

		// real parser wrapper
		template<
			typename Result,                                // 返り値型
			typename Value = typename value<Result>::type,  // 数値型
			typename Policy = qi::real_policies<Value>
		>
		struct real_parser {
			typedef Result  result_type;  // 返り値型
			typedef Value   value_type;   // 数値型
			typedef qi::real_parser<value_type, Policy> type;  // qi rule型
		};
		// real_parser検出用メタクラス
		template<typename T>
		class is_real_parser {
			template<typename N, typename V, typename P>
			static std::true_type test(real_parser<N,V,P> &&);
			static std::false_type test(...);
		public:
			//static const bool value = decltype( test(T()) )::value;
			static const bool value = std::is_same< decltype(test(T())), std::true_type >::value;
		};

	}//namespace lexical_cast_detail

	// XX進数解析ルール
	// 自動で違う進数とみなしたりはしない
	// 16進
	typedef lexical_cast_detail::int_parser<16,  std::uint8_t, 1, -1> as_hex8;
	typedef lexical_cast_detail::int_parser<16, std::uint16_t, 1, -1> as_hex16;
	typedef lexical_cast_detail::int_parser<16, std::uint32_t, 1, -1> as_hex32;
	typedef lexical_cast_detail::int_parser<16, std::uint64_t, 1, -1> as_hex64;
	typedef as_hex32 as_hex;
	// 16進 optional
	typedef lexical_cast_detail::int_parser<16, boost::optional< std::uint8_t>, 1, -1> as_hex8_opt;
	typedef lexical_cast_detail::int_parser<16, boost::optional<std::uint16_t>, 1, -1> as_hex16_opt;
	typedef lexical_cast_detail::int_parser<16, boost::optional<std::uint32_t>, 1, -1> as_hex32_opt;
	typedef lexical_cast_detail::int_parser<16, boost::optional<std::uint64_t>, 1, -1> as_hex64_opt;
	typedef as_hex32_opt as_hex_opt;
	// 8進数
	typedef lexical_cast_detail::int_parser<8,  std::uint8_t, 1, -1> as_oct8;
	typedef lexical_cast_detail::int_parser<8, std::uint16_t, 1, -1> as_oct16;
	typedef lexical_cast_detail::int_parser<8, std::uint32_t, 1, -1> as_oct32;
	typedef lexical_cast_detail::int_parser<8, std::uint64_t, 1, -1> as_oct64;
	typedef as_oct32 as_oct;
	// 8進数 optional
	typedef lexical_cast_detail::int_parser<8, boost::optional< std::uint8_t>, 1, -1> as_oct8_opt;
	typedef lexical_cast_detail::int_parser<8, boost::optional<std::uint16_t>, 1, -1> as_oct16_opt;
	typedef lexical_cast_detail::int_parser<8, boost::optional<std::uint32_t>, 1, -1> as_oct32_opt;
	typedef lexical_cast_detail::int_parser<8, boost::optional<std::uint64_t>, 1, -1> as_oct64_opt;
	typedef as_oct32_opt as_oct_opt;
	// 2進数
	typedef lexical_cast_detail::int_parser<2,  std::uint8_t, 1, -1> as_bin8;
	typedef lexical_cast_detail::int_parser<2, std::uint16_t, 1, -1> as_bin16;
	typedef lexical_cast_detail::int_parser<2, std::uint32_t, 1, -1> as_bin32;
	typedef lexical_cast_detail::int_parser<2, std::uint64_t, 1, -1> as_bin64;
	typedef as_bin32 as_bin;
	// 2進数 optional
	typedef lexical_cast_detail::int_parser<2, boost::optional< std::uint8_t>, 1, -1> as_bin8_opt;
	typedef lexical_cast_detail::int_parser<2, boost::optional<std::uint16_t>, 1, -1> as_bin16_opt;
	typedef lexical_cast_detail::int_parser<2, boost::optional<std::uint32_t>, 1, -1> as_bin32_opt;
	typedef lexical_cast_detail::int_parser<2, boost::optional<std::uint64_t>, 1, -1> as_bin64_opt;
	typedef as_bin32_opt as_bin_opt;

	// 10進数
	// 整数
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
	// 実数
	typedef lexical_cast_detail::real_parser<float>  as_float;
	typedef lexical_cast_detail::real_parser<double> as_double;
	// 10進数 optional
	// 整数
	typedef lexical_cast_detail::int_parser<10, boost::optional<  std::int8_t>, 1, -1>   as_int8_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional< std::uint8_t>, 1, -1>  as_uint8_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional< std::int16_t>, 1, -1>  as_int16_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional<std::uint16_t>, 1, -1> as_uint16_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional< std::int32_t>, 1, -1>  as_int32_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional<std::uint32_t>, 1, -1> as_uint32_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional< std::int64_t>, 1, -1>  as_int64_opt;
	typedef lexical_cast_detail::int_parser<10, boost::optional<std::uint64_t>, 1, -1> as_uint64_opt;
	typedef as_uint32_opt as_uint_opt;
	typedef as_int32_opt   as_int_opt;
	// 実数
	typedef lexical_cast_detail::real_parser< boost::optional<float>  >  as_float_opt;
	typedef lexical_cast_detail::real_parser< boost::optional<double> > as_double_opt;

}// namespace lulib


