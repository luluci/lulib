#pragma once

#include <string>
#include <lulib/unicode/traits/codepoint.hpp>

namespace lulib { namespace unicode_detail { namespace traits {

	struct utf16 {
		typedef char16_t         char_type;
		typedef std::u16string string_type;

		// コードポイントを取得する
		// イテレータは次の文字の開始位置にまでforwardされる
		// First byte の解析
		//    : First byte は先頭に "10" を取らない
		template<typename Iterator>
		static codepoint get_code_point(Iterator &it, Iterator end) {
			codepoint result;
			if (it == end) return result;

			const std::size_t low_mask  = 0xDC00;  // 下位バイトマスク
			const std::size_t high_mask = 0xD800;  // 上位バイトマスク
			std::uint32_t cp, rest;

			// 値が U+0000~U+D7FF, U+E000~U+FFFF : 2バイト
			if (
				(0x0000 <= *it && *it <= 0xD7FF)
				|| (0xE000 <= *it && *it <= 0xFFFF)
			) {
				result.value     = *it;
				result.bit_size  = 16;
				result.byte_size = 2;
			}
			// 上位6bitが high_mask なら、4バイト
			else if ((*it & 0xFC00) == high_mask) {
				// 再上位バイト復元
				cp = ((*it & 0x03C0) >> 6);    // 1101 10ww ww00 0000 の wwwwを抽出
				cp += 1;                       // wwww = uuuuu - 1
				cp <<= 16;                     // 000u uuuu xxxx xxxx xxxx xxxx
				// 次のバイト復元
				cp |= ((*it & 0x003F) << 10);  // 1101 10ww wwxx xxxx の xxxxxx を抽出
				// 残りバイト復元
				++it;
				if (it == end) return result;
				cp |= (*it & 0x03FF);          // 1101 11xx xxxx xxxx の xx xxxx xxxx を抽出
				result.value = cp;
				result.bit_size  = 21;
				result.byte_size = 3;
			}
			// 不正なシーケンス
			else {
				return result;
			}
			++it;
			return result;
		}

		// code-point からエンコードに必要な bit/byte数を返す
		static codepoint get_size(std::uint32_t cp) {
			codepoint result;

			if (
				(0x00000000 <= cp && cp <= 0x0000D7FF)
				|| (0x0000E000 <= cp && cp <= 0x0000FFFF)
			) {
				result.bit_size  = 16;
				result.byte_size = 2;
			}
			else if (0x00010000 <= cp && cp <= 0x0010FFFF) {
				result.bit_size  = 21;
				result.byte_size = 4;
			}
			else {
				result.bit_size  = 0;
				result.byte_size = 0;
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
			std::uint32_t temp, temp2;
			std::size_t byte_size = get_byte_size(cp);
			str.reserve(byte_size);

			switch (byte_size) {
				case 2: {
					str.push_back( static_cast<char_type>(cp) );
					break;
				}
				case 4: {
					// 00000000 000zzzzz yyyyyyxx xxxxxxxx
					// 00000000 00011111 00000000 00000000 => 0x001F0000
					// 00000000 00000000 11111100 00000000 => 0x0000FC00
					// 00000000 00000000 00000011 11111111 => 0x000003FF
					// wwww = zzzzz - 1
					// 1101 10ww wwyy yyyy
					// 1101 11xx xxxx xxxx
					// 第2byte
					temp = cp & 0x001F0000;
					temp >>= 16;
					temp -= 1;
					temp2 = cp & 0x0000FC00;
					temp2 >>= 10;
					temp |= 0xD800 | temp2;
					str.push_back( static_cast<char_type>(temp) );
					// 第1byte
					temp = cp & 0x000003FF;
					temp |= 0xDC00;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
			}

			return str;
		}
	};


}}}//namespace lulib::unicode_detail::traits

