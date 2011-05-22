#pragma once
#pragma warning(disable : 4819)

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace font_detail { namespace attribute {

	template<typename Char = TCHAR>
	struct basic_face {
		// Char型特性
		typedef lulib::char_traits<Char>          char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

	public:
		basic_face(char_type const* str) : face_(str) {}
		basic_face(string_type && str) : face_( std::move(str) ) {}
		basic_face(string_type const& str) : face_(str) {}

		inline string_type&& operator()() { return std::move(face_); }

	private:
		string_type face_;  // タイトルバーに表示されるテキスト
	};

}}}}// namespace lulib::win32api::window_detail::attribute
