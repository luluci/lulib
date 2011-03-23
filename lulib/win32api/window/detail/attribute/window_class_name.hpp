#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_class.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	template<typename Char = TCHAR>
	struct basic_window_class_name {
		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;
		// WindowClass
		typedef basic_window_class<Char> wnd_class;

	public:
		basic_window_class_name(char_type const* t) : wnd_class_(t) {}
		basic_window_class_name(string_type && t) : wnd_class_( std::move(t) ) {}
		basic_window_class_name(string_type const& t) : wnd_class_(t) {}
		basic_window_class_name(wnd_class const& wc) : wnd_class_(wc.class_name()) {}

		inline operator std::string& () { return wnd_class_; }

	private:
		string_type wnd_class_;  // タイトルバーに表示されるテキスト
	};

}}}}// namespace lulib::win32api::window_detail::attribute
