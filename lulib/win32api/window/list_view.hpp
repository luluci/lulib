#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_list_view.hpp>

namespace lulib { namespace win32api {

	// Unicodeフラグ(TCHAR)に依存したデフォルトListView型
	typedef basic_list_view<>        list_view;
	// マルチバイト文字ListView型
	typedef basic_list_view<char>    list_view_a;
	// ワイド文字ListView型
	typedef basic_list_view<wchar_t> list_view_w;

}}// namespace lulib::win32api
