#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace menu {

	// forward
	template<HMENU (WINAPI *create_menu)(), typename Char>
	class basic_menu;

}}}// namespace lulib::win32api::menu

