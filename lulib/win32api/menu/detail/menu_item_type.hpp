#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace menu { namespace detail {

	// mask
	struct mask {
		enum enum_t {
			state      = MIIM_STATE,
			id         = MIIM_ID,
			submenu    = MIIM_SUBMENU,
			checkmarks = MIIM_CHECKMARKS,
			//type     = MIIM_TYPE, // deprecated
			data       = MIIM_DATA,
			string     = MIIM_STRING,
			bitmap     = MIIM_BITMAP,
			ftype      = MIIM_FTYPE,
		};
	};

	// ftype
	struct ftype {
		enum enum_t {
			//string      = MFT_STRING, // deprecated
			//bitmap      = MFT_BITMAP, // deprecated
			menubar_break = MFT_MENUBARBREAK,
			menu_break    = MFT_MENUBREAK,
			owner_draw    = MFT_OWNERDRAW,
			radio_check   = MFT_RADIOCHECK,
			separator     = MFT_SEPARATOR,
			right_order   = MFT_RIGHTORDER,
			right_justify = MFT_RIGHTJUSTIFY,
		};
	};

	// state
	struct state {
		enum enum_t {
			enable   = MFS_ENABLED,  // 通常表示
			disabled = MFS_DISABLED,  // アイテムを灰色で表示し、選択不可にする
			checked  = MFS_CHECKED,  // アイテムにチェックマークをつける
			hilite   = MFS_HILITE,  // アイテムをハイライト表示
			defitem  = MFS_DEFAULT,  // デフォルトアイテムに設定します。デフォルトアイテムは1つのメニューに対して1つまで持つことができます。デフォルトアイテムは太字で表示されます。
		};
	};


}}}}// namespace lulib::win32api::menu::detail
