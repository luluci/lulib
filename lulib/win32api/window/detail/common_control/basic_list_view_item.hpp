#pragma once
#pragma warning(disable : 4819)

//#include <windows.h>
#include <commctrl.h>

#include <lulib/win32api/window/detail/list_view_policy.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// LVCOL構造体ラッパ
	template<typename Char = TCHAR>
	class basic_list_view_item {
		// self_type
		typedef basic_list_view_item<Char> self_type;

		// ListViewポリシー
		typedef list_view_policy<Char> lv_policy;
		// 列型
		typedef typename lv_policy::lv_item lv_item;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

	public:
		basic_list_view_item() : item_(), text_() {
		}

		operator lv_item const*() const {
			return &item_;
		}

		// item
		// 何行目のアイテムか
		void item(int row) { item_.iItem = row; }
		// alias
		void row(int r) { item_.iItem = r; }
		void row_index(int row) { item_.iItem = row; }
		int row() const { return item_.iItem; }
		// subitem
		// 何列目のアイテムか
		void subitem(int col) { item_.iSubItem = col; }
		// alias
		void col(int c) { item_.iSubItem = c; }
		void col_index(int col) { item_.iSubItem = col; }
		int col() const { return item_.iSubItem; }
		// 一括指定
		void pos(int row, int col) {
			item_.iItem = row;
			item_.iSubItem = col;
		}

		// text
		void text(char_type const* text) {
			item_.mask |= LVIF_TEXT;
			text_ = text;
			item_.pszText = const_cast<char_type*>(text_.c_str());
		}
		void text(string_type const& text) {
			item_.mask |= LVIF_TEXT;
			text_ = text;
			item_.pszText = const_cast<char_type*>(text_.c_str());
		}

		// state
		struct state_flag {
			enum enum_t {
				focused      = LVIS_FOCUSED,
				selected     = LVIS_SELECTED,
				cut          = LVIS_CUT,
				drop_hilited = LVIS_DROPHILITED,
				//glow         = LVIS_GLOW,
				//activating   = LVIS_ACTIVATING,
			};
		};
		void state(UINT s) {
			item_.mask |= LVIF_STATE;
			item_.state = s;
		}
		void state_mask(UINT mask) {
			item_.stateMask = mask;
		}

		// image
		void image(int img) {
			item_.mask |= LVIF_IMAGE;
			item_.iImage = img;
		}

		// indent
		void indent(int ind) {
			item_.mask |= LVIF_INDENT;
			item_.iIndent = ind;
		}

		// lParam
		void param(LPARAM lp) {
			item_.mask |= LVIF_PARAM;
			item_.lParam = lp;
		}

	private:
		lv_item item_;
		string_type text_;
	};

}}}}// namespace lulib::win32api::window_detail::common_control
