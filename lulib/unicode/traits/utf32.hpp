#pragma once

#include <string>
#include <lulib/unicode/traits/codepoint.hpp>

namespace lulib { namespace unicode_detail { namespace traits {

	struct utf32 {
		typedef char32_t         char_type;
		typedef std::u32string string_type;

		// コードポイントを取得する
		// イテレータは次の文字の開始位置にまでforwardされる
		// First byte の解析
		//    : First byte は先頭に "10" を取らない
		template<typename Iterator>
		static codepoint get_code_point(Iterator &it, Iterator end) {
			codepoint result;
			if (it == end) return result;

			// Unicode code-point: U+00000000 .. U+0010FFFF
			if (0x00000000 <= *it && *it <= 0x0010FFFF) {
				result.value     = *it;
				result.bit_size  = 21;
				result.byte_size = 4;
				++it;
			}

			return result;
		}

		// code-point からエンコードに必要な bit/byte数を返す
		static codepoint get_size(std::uint32_t cp) {
			codepoint result;

			// Unicode code-point: U+00000000 .. U+0010FFFF
			if (0x00000000 <= cp && cp <= 0x0010FFFF) {
				result.bit_size  = 21;
				result.byte_size = 4;
			}

			return result;
		}
		inline static std::size_t get_bit_size(std::uint32_t cp) {
			return get_size(cp).bit_size;
		}
		inline static std::size_t get_byte_size(std::uint32_t cp) {
			return get_size(cp).byte_size;
		}

		static string_type get_string(std::uint32_t cp) {
			string_type str;
			std::size_t byte_size = get_byte_size(cp);
			str.reserve(byte_size);

			switch (byte_size) {
				case 4: {
					str.push_back( static_cast<char_type>(cp) );
					break;
				}
			}

			return str;
		}
	};


}}}//namespace lulib::unicode_detail::traits

