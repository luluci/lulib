#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>

namespace lulib { namespace win32api { namespace menu { namespace item {

	// menu item forward
	template<typename T> struct basic_string;
	template<typename T> struct basic_submenu;
	struct separator;

	// friend関数forward
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, basic_string<T> &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, basic_submenu<T> &&);
	template<HMENU (WINAPI *C)(), typename T>
	basic_menu<C,T>& operator<<(basic_menu<C,T>&, separator &&);

}}}}// namespace lulib::win32api::menu::item
