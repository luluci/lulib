#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

#include <boost/optional.hpp>

#include <lulib/mpl/size.hpp>
#include <lulib/type_traits/type_switch.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/unicode/traits.hpp>

namespace lulib {

	namespace unicode_detail {

		// Unicode CodePoint type
		struct codepoint {
			// code point type
			typedef std::uint32_t value_type;
			// encoding type
			typedef traits::utf8::string_type  utf8_type;
			typedef traits::utf16::string_type utf16_type;
			typedef traits::utf32::string_type utf32_type;

			// sizeof(Iterator::value_type) == 1(byte,  8bit) => UTF-8
			// sizeof(Iterator::value_type) == 2(byte, 16bit) => UTF-16
			// sizeof(Iterator::value_type) == 4(byte, 32bit) => UTF-32
			typedef type_switch<
				type_case< mpl::size<1>::equal_to_sizeof<lulib::_1>, traits::utf8 >,
				type_case< mpl::size<2>::equal_to_sizeof<lulib::_1>, traits::utf16 >,
				type_case< mpl::size<4>::equal_to_sizeof<lulib::_1>, traits::utf32 >
			> switch_encode_t;

		public:
			codepoint()
			: value_(0), is_valid_(false) {
			}
			codepoint(value_type cp)
			: value_(cp), is_valid_( validation(cp) ) {
			}
			template<typename Iterator>
			codepoint(Iterator &it, Iterator end)
			: value_(0), is_valid_(false) {
				convert(it, end);
			}

		public:
			// Modifier
			// code-point セット
			codepoint& operator=(std::uint32_t cp) {
				value_ = cp;
				is_valid_ = validation(cp);
				return *this;
			}
			// UTF-XX から code-point を計算
			template<typename Iterator>
			void convert(Iterator &it, Iterator end) {
				typedef typename char_traits<Iterator>::char_type char_type;
				typedef typename switch_encode_t::case_<char_type>::type encode_type;

				// codepoint情報を取得
				auto cp = encode_type::get_code_point(it, end);
				if (cp.byte_size != 0) {
					value_ = cp.value;
					is_valid_ = true;
				}
			}

		public:
			inline operator bool() const { return is_valid_; }

			// code-point validation
			inline bool validation(std::uint32_t cp) const {
				if (0x00000000 <= cp && cp <= 0x0010FFFF) return true;
				else return false;
			}

		public:
			// Element Access
			inline operator    value_type()  const { return value_; }
			inline value_type       value()  const { return value_; }
			// code-point convert to UTF-8
			inline utf8_type utf8() const {
				return traits::utf8::get_string(value_);
			}
			// code-point convert to UTF-16
			inline utf16_type utf16() const {
				return traits::utf16::get_string(value_);
			}
			// code-point convert to UTF-32
			inline utf32_type utf32() const {
				return traits::utf32::get_string(value_);
			}

			// UTF-8 byte size
			inline std::size_t utf8_size() const {
				return traits::utf8::get_byte_size(value_);
			}
			// UTF-16 byte size
			inline std::size_t utf16_size() const {
				return traits::utf16::get_byte_size(value_);
			}
			// UTF-32 byte size
			inline std::size_t utf32_size() const {
				return traits::utf32::get_byte_size(value_);
			}

		private:
			value_type value_;  // code point
			utf8_type  utf8_;   // UTF-8  byte sequence
			utf16_type utf16_;  // UTF-16 byte sequence
			bool is_valid_;     // 有効な byte sequence がセットされたか
		};

		template<typename OutputStream>
		OutputStream& operator<<(OutputStream &ost, codepoint const& cp) {
			ost << cp.value();
			return ost;
		}

		template<typename ForwardSequence>
		codepoint& operator<<(codepoint &cp, ForwardSequence const& seq) {
			using std::begin;
			using std::end;

			auto it = begin(seq);
			cp.convert(it, end(seq));
			return cp;
		}

	}// namespace unicode_detail

	using unicode_detail::codepoint;

}// namespace lulib

