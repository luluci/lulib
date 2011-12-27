#pragma once
#pragma warning(disable : 4819)

#include <cstdint>
#include <vector>
#include <iterator>

#include <lulib/type_traits/char_traits.hpp>

#include <lulib/unicode/codepoint.hpp>
#include <lulib/unicode/traits.hpp>

namespace lulib {

	namespace unicode_detail {

		// Unicode
		//   impls as code-point sequence
		class unicode {

		public:
			typedef traits::utf8                   utf8_traits;
			typedef lulib::char_traits<char>       utf8_char_traits;
			typedef utf8_char_traits::char_type    utf8_char;
			typedef utf8_char_traits::string_type  utf8_string;
			typedef traits::utf16                  utf16_traits;
			typedef lulib::char_traits<char16_t>   utf16_char_traits;
			typedef utf16_char_traits::char_type   utf16_char;
			typedef utf16_char_traits::string_type utf16_string;
			typedef traits::utf32                  utf32_traits;
			typedef lulib::char_traits<char32_t>   utf32_char_traits;
			typedef utf32_char_traits::char_type   utf32_char;
			typedef utf32_char_traits::string_type utf32_string;

			// code-point sequence
			typedef std::vector<codepoint>         container;
			typedef container::iterator             iterator;
			typedef container::const_iterator const_iterator;

		public:
			unicode() : c_(), utf8_size_(0), utf16_size_(0) {}
			template<typename String>
			unicode(String const& str) : c_(), utf8_size_(0), utf16_size_(0) {
				convert(str);
			}

		public:
			// Element Access
			iterator begin() { return c_.begin(); }
			iterator end()   { return c_.end();   }
			const_iterator begin() const { return c_.begin(); }
			const_iterator end()   const { return c_.end();   }

			inline std::size_t utf8_size()  const { return utf8_size_; }
			inline std::size_t utf16_size() const { return utf16_size_; }
			inline std::size_t utf32_size() const { return c_.size() * 4; }

			utf8_string utf8() {
				utf8_string str;
				str.reserve( c_.size() * 4 );
				auto it = c_.begin();
				auto end = c_.end();
				while (it != end) {
					str += it->utf8();
					++it;
				}
				return str;
			}
			utf16_string utf16() {
				utf16_string str;
				str.reserve( c_.size() * 2 );
				auto it = c_.begin();
				auto end = c_.end();
				while (it != end) {
					str += it->utf16();
					++it;
				}
				return str;
			}
			utf32_string utf32() {
				utf32_string str;
				str.reserve( c_.size() );
				auto it = c_.begin();
				auto end = c_.end();
				while (it != end) {
					str += it->utf32();
					++it;
				}
				return str;
			}

		public:
			// Modifier
			template<typename Char, std::size_t N>
			void convert(Char (&str)[N]) {
				using std::begin;
				using std::end;
				auto it = begin(str);
				auto ie = end(str) - 1;  // \0 を除く
				convert_impl(it, ie, N);
			}
			template<typename String>
			void convert(String const& str) {
				using std::begin;
				using std::end;
				auto it = begin(str);
				auto ie = end(str);
				convert_impl(it, ie, str.size());
			}
			template<typename Iterator>
			void convert(Iterator it, Iterator end) {
				convert_impl( it, end, std::distance(it, end) );
			}

		private:
			template<typename Iterator>
			void convert_impl(Iterator &it, Iterator end, std::size_t size) {
				codepoint cp;

				utf8_size_ = 0;
				utf16_size_ = 0;
				c_.clear();          // 新しくコンテナを作成
				c_.reserve( size );  // 元の文字列と同じくらいの長さはいるはず

				while (it != end) {
					cp.convert(it, end);
					if (cp) {
						utf8_size_ += cp.utf8_size();
						utf16_size_ += cp.utf16_size();
						c_.push_back(cp);
					}
					else {
						return;
					}
				}
			}

		private:
			container c_;
			std::size_t utf8_size_, utf16_size_;  // 文字列長
		};

	}// namespace lulib::unicode_detail

	using unicode_detail::unicode;

}// namespace lulib

