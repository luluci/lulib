#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace menu_detail {

	// forward
	template<HMENU (WINAPI *create_menu)(), typename Char> class basic_menu;

}}}// namespace lulib::win32api::menu
