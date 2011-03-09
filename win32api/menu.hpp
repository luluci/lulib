#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu.hpp>

namespace lulib { namespace win32api {

	typedef menu::basic_menu< ::CreateMenu > menu_type;
	typedef menu::basic_menu< ::CreatePopupMenu > popup_menu_type;

}}// namespace lulib::win32api
