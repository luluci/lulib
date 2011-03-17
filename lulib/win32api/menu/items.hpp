#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/item/string.hpp>
#include <lulib/win32api/menu/item/submenu.hpp>
#include <lulib/win32api/menu/item/separator.hpp>

namespace lulib { namespace win32api { namespace menu {

	// 属性
	// 文字列メニューアイテム
	typedef item::basic_string<TCHAR>    string;
	typedef item::basic_string<char>     string_a;
	typedef item::basic_string<wchar_t>  string_w;
	// サブメニューアイテム
	typedef item::basic_submenu<TCHAR>    submenu;
	typedef item::basic_submenu<char>     submenu_a;
	typedef item::basic_submenu<wchar_t>  submenu_w;
	// セパレータアイテム
	typedef item::separator    separator;

}}}// namespace lulib::win32api::window
