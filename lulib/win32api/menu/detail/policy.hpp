#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace menu_detail {

	// Menuポリシー
	template<typename Char>
	struct policy;

	template<>
	struct policy<char> {
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
	struct policy<wchar_t> {
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

}}}// namespace lulib::win32api::menu_detail
