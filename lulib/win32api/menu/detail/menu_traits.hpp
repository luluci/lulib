#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <string>

namespace lulib { namespace win32api { namespace menu { namespace detail {

	// char 特性
	template<typename Char>
	struct menu_traits;

	template<>
	struct menu_traits<char> {
		// MENUITEMINFO
		typedef MENUITEMINFOA menu_item_info_type;

		// ::InsertMenuItem
		inline static BOOL insert_menu_item(
			HMENU hMenu,
			UINT item,
			BOOL fByPosition,
			LPCMENUITEMINFOA lpmi
		) {
			return ::InsertMenuItemA(hMenu, item, fByPosition, lpmi);
		}

		// ::SetMenuItemInfo
		inline static BOOL set_menu_item_info(
			HMENU hMenu,
			UINT item,
			BOOL fByPosition,
			LPCMENUITEMINFOA lpmii
		) {
			return ::SetMenuItemInfoA(hMenu, item, fByPosition, lpmii);
		}
	};

	template<>
	struct menu_traits<wchar_t> {
		// MENUITEMINFO
		typedef MENUITEMINFOW menu_item_info_type;

		// ::InsertMenuItem
		inline static BOOL insert_menu_item(
			HMENU hMenu,
			UINT item,
			BOOL fByPosition,
			LPCMENUITEMINFOW lpmii
		) {
			return ::InsertMenuItemW(hMenu, item, fByPosition, lpmii);
		}

		// ::SetMenuItemInfo
		inline static BOOL set_menu_item_info(
			HMENU hMenu,
			UINT item,
			BOOL fByPosition,
			LPCMENUITEMINFOW lpmii
		) {
			return ::SetMenuItemInfoW(hMenu, item, fByPosition, lpmii);
		}
	};

}}}}// namespace lulib::win32api::menu::detail
