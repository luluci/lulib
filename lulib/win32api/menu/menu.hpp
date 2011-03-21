#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu.hpp>

namespace lulib { namespace win32api {

	// Unicodeフラグ(TCHAR)に依存したデフォルトメニュー型
	typedef basic_menu< ::CreateMenu >            menu;
	typedef basic_menu< ::CreatePopupMenu > popup_menu;

	// マルチバイト文字メニュー型
	typedef basic_menu< ::CreateMenu,      char >       menu_a;
	typedef basic_menu< ::CreatePopupMenu, char > popup_menu_a;

	// ワイド文字メニュー型
	typedef basic_menu< ::CreateMenu,      wchar_t >       menu_w;
	typedef basic_menu< ::CreatePopupMenu, wchar_t > popup_menu_w;

}}// namespace lulib::win32api
