#pragma once
#pragma warning(disable : 4819)

#include <string>
#include <lulib/unicode/traits/codepoint.hpp>

namespace lulib { namespace unicode_detail { namespace traits {

	struct utf8 {
		typedef char        char_type;
		typedef std::string string_type;

		// コードポイントを取得する
		// イテレータは次の文字の開始位置にまでforwardされる
		// First byte の解析
		//    : First byte は先頭に "10" を取らない
		template<typename Iterator>
		static codepoint get_code_point(Iterator &it, Iterator end) {
			codepoint result;
			if (it == end) return result;

			std::uint32_t cp, rest;

			// 上位1bitが0なら、1バイト文字
			if ((*it & 0x80) == 0x00) {
				cp = (*it & 0x7F);  // 有効bit(下位7bit)を取得
				rest = 0;
				result.bit_size  = 7;
				result.byte_size = 1;
			}
			// 上位3bitが110 (110X XXXX) なら、2バイト文字
			else if ((*it & 0xE0) == 0xC0) {
				cp = (*it & 0x1F);  // 有効bit(下位3bit)を取得
				rest = 1;
				result.bit_size  = 11;
				result.byte_size = 2;
			}
			// 上位4bitが1110 (1110 XXXX) なら、3バイト文字
			else if ((*it & 0xF0) == 0xE0) {
				cp = (*it & 0x0F);  // 有効bit(下位3bit)を取得
				rest = 2;
				result.bit_size  = 16;
				result.byte_size = 3;
			}
			// 上位5bitが11110 (1111 0XXX) なら、4バイト文字
			else if ((*it & 0xF8) == 0xF0) {
				cp = (*it & 0x07);  // 有効bit(下位3bit)を取得
				rest = 3;
				result.bit_size  = 21;
				result.byte_size = 4;
			}
			// 不正なシーケンス
			else {
				return result;
			}
			// ASCIIでなく有効bitが0: 冗長な表現のため、不正なシーケンス
			if (result.bit_size != 7 && cp == 0) {
				return result;
			}

			// 残りbyte-sequenceからコードポイントを求める
			++it;
			result.value = get_code_point_impl(it, end, cp, rest);
			return result;
		}
		// continuation bytes の解析
		//     : continuation byte は必ず "10" で始まる
		//   cp: 先頭バイトの有効データ
		// rest: 残りバイト
		template<typename Iterator>
		static std::size_t get_code_point_impl(
			Iterator &it, Iterator end, std::uint32_t cp, std::uint32_t rest
		) {
			for (; rest > 0 && it != end && (*it & 0xC0) == 0x80; --rest) {
				cp = ((cp << 6) | (*it & 0x3F));  // 下位byteの下位6bitを連結
				++it;
			}
			// start byte が示す数の continuation bytes がない
			if (rest > 0) return 0;
			return cp;
		}

		// code-point からエンコードに必要な bit/byte数を返す
		static codepoint get_size(std::uint32_t cp) {
			codepoint result;

			// 1byte: U+00 .. U+7F
			if (0x00000000 <= cp && cp <= 0x0000007F) {
				result.bit_size  = 7;
				result.byte_size = 1;
			}
			// 2byte: U+0080 .. U+ 07FF
			else if (0x00000080 <= cp && cp <= 0x000007FF) {
				result.bit_size  = 11;
				result.byte_size = 2;
			}
			// 3byte: U+0800 .. U+FFFF
			else if (0x00000800 <= cp && cp <= 0x0000FFFF) {
				result.bit_size  = 16;
				result.byte_size = 3;
			}
			// 4byte: U+010000 .. U+1FFFFF
			else if (0x00010000 <= cp && cp <= 0x001FFFFF) {
				result.bit_size  = 21;
				result.byte_size = 4;
			}
			/*
			// invalide sequence
			// 5byte: U+00200000 .. U+03FFFFFF
			else if (0x00200000 <= value_ && value_ <= 0x03FFFFFF) {
				result.bit_size  = 26;
				result.byte_size = 5;
			}
			// 6byte: U+04000000 .. U+7FFFFFFF
			else if (0x04000000 <= value_ && value_ <= 0x7FFFFFFF) {
				result.bit_size  = 31;
				result.byte_size = 6;
				return 6;
			}
			*/
			// error: その他
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

		static string_type get_string(std::uint32_t value) {
			string_type str;
			std::uint32_t temp;
			codepoint cp = get_size(value);
			str.reserve(cp.byte_size);

			switch (cp.byte_size) {
				case 1: {
					// 第1byte
					str.push_back( static_cast<char_type>(value) );
					break;
				}
				case 2: {
					// 00000000 00000000 00000yyy yyxxxxxx
					// 00000000 00000000 00000111 11000000 => 0x000007C0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 110y yyyy
					// 10xx xxxx
					// 第2byte
					temp = value & 0x000007C0;
					temp >>= 6;
					temp |= 0xC0;
					str.push_back( static_cast<char_type>(temp) );
					// 第1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
				case 3: {
					// 00000000 00000000 zzzzyyyy yyxxxxxx
					// 00000000 00000000 11110000 00000000 => 0x0000F000
					// 00000000 00000000 00001111 11000000 => 0x00000FC0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 1110 zzzz
					// 10yy yyyy
					// 10xx xxxx
					// 第3byte
					temp = value & 0x0000F000;
					temp >>= 12;
					temp |= 0xE0;
					str.push_back( static_cast<char_type>(temp) );
					// 第2byte
					temp = value & 0x00000FC0;
					temp >>= 6;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// 第1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
				case 4: {
					// 00000000 000aaazz zzzzyyyy yyxxxxxx
					// 00000000 00011100 00000000 00000000 => 0x001C0000
					// 00000000 00000011 11110000 00000000 => 0x0003F000
					// 00000000 00000000 00001111 11000000 => 0x00000FC0
					// 00000000 00000000 00000000 00111111 => 0x0000003F
					// 1111 0aaa
					// 10zz zzzz
					// 10yy yyyy
					// 10xx xxxx
					// 第4byte
					temp = value & 0x001C0000;
					temp >>= 18;
					temp |= 0xF0;
					str.push_back( static_cast<char_type>(temp) );
					// 第3byte
					temp = value & 0x0003F000;
					temp >>= 12;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// 第2byte
					temp = value & 0x00000FC0;
					temp >>= 6;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					// 第1byte
					temp = value & 0x0000003F;
					temp |= 0x80;
					str.push_back( static_cast<char_type>(temp) );
					break;
				}
			}

			return str;
		}
	};
}}}// namespace lulib::unicode_detail::traits
