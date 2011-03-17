#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>

namespace lulib { namespace win32api { namespace menu { namespace state {

	// menu item forward
	struct enable;
	struct disabled;
	struct checked;
	struct hilite;
	struct defitem;
	struct multi;

	// friend関数forward
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, enable &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, disabled &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, checked &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, hilite &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, defitem &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, multi &&);

}}}}// namespace lulib::win32api::menu::item
