#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_static_text.hpp>

namespace lulib { namespace win32api {

	// Unicodeフラグ(TCHAR)に依存したデフォルトListView型
	typedef basic_static_text<>        static_text;
	// マルチバイト文字ListView型
	typedef basic_static_text<char>    static_text_a;
	// ワイド文字ListView型
	typedef basic_static_text<wchar_t> static_text_w;

}}// namespace lulib::win32api
