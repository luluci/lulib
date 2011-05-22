#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/font/basic_font.hpp>

namespace lulib { namespace win32api {
	// Font型
	typedef basic_font<> font;
	typedef basic_font<char> font_a;
	typedef basic_font<wchar_t> font_w;
}}// namespace lulib::win32api
