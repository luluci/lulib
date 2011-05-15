#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

namespace lulib { namespace win32api { namespace window_detail {

	// Windowポリシー
	template<typename Char>
	struct list_view_policy;

	template<>
	struct list_view_policy<char> {
		// LV_COLUMN
		typedef LV_COLUMNA lv_column;
		// LV_ITEM
		typedef LV_ITEMA lv_item;

		// WindowClassName
		static char const* wnd_class;

		// ListView_GetExtendedListViewStyle
		static DWORD get_ex_style(HWND hWnd) {
			// ListView_GetExtendedListViewStyle(hWnd);
			return ::SendMessage(hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
		}
		// ListView_SetExtendedListViewStyle
		static void set_ex_style(HWND hWnd, DWORD ex_style) {
			// ListView_SetExtendedListViewStyle(hWnd, ex_style);
			::SendMessage(hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, ex_style);
		}

		// ListView_InsertColumn
		static int insert_column(HWND hWnd, int iCol, lv_column const* pcol) {
			//ListView_InsertColumn(hWnd, iCol, pcol);
			return ::SendMessage( hWnd, LVM_INSERTCOLUMNA, iCol, reinterpret_cast<LPARAM>(pcol) );
		}
		// ListView_InsertItem
		static int insert_item(HWND hWnd, lv_item const* item) {
			// ListView_InsertItem(hWnd, item);
			return ::SendMessage( hWnd, LVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(item) );
		}

		// ListView_GetItem
		static BOOL get_item(HWND hWnd, lv_item* item) {
			return ::SendMessage(hWnd, LVM_GETITEM, 0, reinterpret_cast<LPARAM>(item));
		}
		// ListView_SetItem
		static BOOL set_item(HWND hWnd, lv_item const* item) {
			return ::SendMessage(hWnd, LVM_SETITEM, 0, reinterpret_cast<LPARAM>(item));
		}
	};
	char const* list_view_policy<char>::wnd_class = WC_LISTVIEWA;

	template<>
	struct list_view_policy<wchar_t> {
		// LV_COLUMN
		typedef LV_COLUMNW lv_column;
		// LV_ITEM
		typedef LV_ITEMW lv_item;

		// WindowClassName
		static wchar_t const* wnd_class;

		// ListView_GetExtendedListViewStyle
		static DWORD get_ex_style(HWND hWnd) {
			// ListView_GetExtendedListViewStyle(hWnd);
			return ::SendMessage(hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
		}
		// ListView_SetExtendedListViewStyle
		static void set_ex_style(HWND hWnd, DWORD ex_style) {
			// ListView_SetExtendedListViewStyle(hWnd, ex_style);
			::SendMessage(hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, ex_style);
		}

		// ListView_InsertColumn
		static int insert_column(HWND hWnd, int iCol, lv_column const* pcol) {
			//ListView_InsertColumn(hWnd, iCol, pcol);
			return ::SendMessage( hWnd, LVM_INSERTCOLUMNW, iCol, reinterpret_cast<LPARAM>(pcol) );
		}
		// ListView_InsertItem
		static int insert_item(HWND hWnd, lv_item const* item) {
			// ListView_InsertItem(hWnd, item);
			return ::SendMessage( hWnd, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(item) );
		}

		// ListView_GetItem
		static BOOL get_item(HWND hWnd, lv_item* item) {
			return ::SendMessage(hWnd, LVM_GETITEM, 0, reinterpret_cast<LPARAM>(item));
		}
		// ListView_SetItem
		static BOOL set_item(HWND hWnd, lv_item const* item) {
			return ::SendMessage(hWnd, LVM_SETITEM, 0, reinterpret_cast<LPARAM>(item));
		}
	};
	wchar_t const* list_view_policy<wchar_t>::wnd_class = WC_LISTVIEWW;

}}}// namespace lulib::win32api::window_detail
