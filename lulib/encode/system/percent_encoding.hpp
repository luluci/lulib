#pragma once
#pragma warning(disable : 4819)

#include <iterator>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib {
	namespace encode_system {

		namespace percent_encoding_detail {
	
			struct encode_impl_1byte {
				template<typename Char, typename String>
				inline static bool encode(Char ch, String &result, bool enc_reserved) {
					// 非予約文字: エンコードしない
					if ( is_unreserved_character(ch) ) {
						result.push_back(ch);
						return false;
					}
					// 予約文字:
					else if ( is_reserved_character(ch) ) {
						// 予約文字エンコード有効: エンコードする
						if (enc_reserved) {
							percent_encoding_impl(ch, result);
							return true;
						}
						// 予約文字エンコード無効: エンコードしない
						else {
							result.push_back(ch);
							return false;
						}
					}
					// 非予約文字, 予約文字以外: エンコードする
					else {
						percent_encoding_impl(ch, result);
						return true;
					}
				}
			};

			// 非予約文字をエスケープ
			template<typename Char, typename String>
			bool encode_impl(Char ch, String &result, bool enc_reserved) {
				// Charのサイズで処理分岐
				// の予定
				return encode_impl_1byte::encode(ch, result, enc_reserved);
			}

			// Unreserved Characters: a-zA-Z0-9-._~
			template<typename Char>
			bool is_unreserved_character(Char ch) {
				// 非予約文字
				if (
					( 'a' <= ch && ch <= 'z' )
					|| ('A' <= ch && ch <= 'Z')
					|| ('0' <= ch && ch <= '9')
					|| (ch == '-')
					|| (ch == '.')
					|| (ch == '_')
					|| (ch == '~')
				) {
					return true;
				}
				// 非予約文字以外
				else {
					return false;
				}
			}
			// Reserved Characters: :/?#[]@!$&'()*+,;=
			//   !
			//   #$
			//   &'()*+,
			//   /
			//   :;
			//   =
			//   ?@
			//   [
			//   ]
			template<typename Char>
			bool is_reserved_character(Char ch) {
				// 予約文字
				if (
					(ch == '!')
					|| ('#' <= ch && ch <= '$')
					|| ('&' <= ch && ch <= ',')
					|| (ch == '/')
					|| (':' <= ch && ch <= ';')
					|| (ch == '=')
					|| ('?' <= ch && ch <= '@')
					|| (ch == '[')
					|| (ch == ']')
				) {
					return true;
				}
				// 非予約文字以外: エスケープする
				else {
					return false;
				}
			}

			template<typename Char, typename String>
			void percent_encoding_impl(Char ch, String &result) {
				Char tmp;
				// 1文字目
				result.push_back('%');
				// 2文字目
				tmp = ((ch & 0xF0) >> 4);
				if (0 <= tmp && tmp <= 9)        result.push_back(tmp + '0');
				else if (10 <= tmp && tmp <= 15) result.push_back(tmp - 10 + 'A');
				// 3文字目
				tmp = (ch & 0x0F);
				if (0 <= tmp && tmp <= 9)        result.push_back(tmp + '0');
				else if (10 <= tmp && tmp <= 15) result.push_back(tmp - 10 + 'A');
			}
		}

		// Percent-Encoding 用タグ
		struct percent_encoding {
			// CharTraits
			typedef lulib::char_traits<char> char_traits;
			typedef char_traits::char_type     char_type;
			typedef char_traits::string_type string_type;

			// Result
			typedef string_type result_type;

			// Encode impl
			// Percent-Encoding detail
			//   非予約文字以外はエンコードする
			//     Unreserved Characters: a-zA-Z0-9-._~
			//       Reserved Characters: :/?#[]@!$&'()*+,;=
			template<typename Iterator>
			static result_type encode(Iterator it, Iterator end, bool enc_reserved = false) {
				string_type buf;
				std::size_t size = std::distance(it, end);
				buf.reserve(size);

				while (it != end) {
					percent_encoding_detail::encode_impl(*it, buf, enc_reserved);
					++it;
				}

				return buf;
			}
		};

	}//namespace lulib::encode_system

	using lulib::encode_system::percent_encoding;

}//namespace lulib
