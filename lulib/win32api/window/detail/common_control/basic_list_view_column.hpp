#pragma once
#pragma warning(disable : 4819)

//#include <windows.h>
#include <commctrl.h>

#include <lulib/win32api/window/detail/common_control/list_view_policy.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// LVCOL構造体ラッパ
	template<typename Char = TCHAR>
	class basic_list_view_column {
		// self_type
		typedef basic_list_view_column<Char> self_type;

		// ListViewポリシー
		typedef list_view_policy<Char> lv_policy;
		// 列型
		typedef typename lv_policy::lv_column lv_column;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

	public:
		basic_list_view_column() : col_(), text_(), index_() {
			init();
		}

		// Default
		void init() {
			// リストビューカラムの初期設定
			col_.mask = LVCF_FMT;
			col_.fmt = LVCFMT_LEFT;
		}

		operator lv_column const*() const {
			return &col_;
		}

		// format
		struct formats {
			enum enum_t {
				left = LVCFMT_LEFT,
				right = LVCFMT_RIGHT,
				center = LVCFMT_CENTER,
				// justify = LVCFMT_JUSTIFYMASK
			};
		};
		void format(int fmt) {
			col_.mask |= LVCF_FMT;
			col_.fmt = fmt;
		}

		// width
		void width(int w) {
			col_.mask |= LVCF_WIDTH;
			col_.cx = w;
		}

		// textをセット
		void text(char_type const* text) {
			col_.mask |= LVCF_TEXT;
			text_ = text;
			col_.pszText = const_cast<char_type*>(text_.c_str());
		}
		void text(string_type const& text) {
			col_.mask |= LVCF_TEXT;
			text_ = text;
			col_.pszText = const_cast<char_type*>(text_.c_str());
		}

		// subitemのインデックス
		// 同じ列のデータはすべてこの値？
		void subitem(int idx) {
			col_.mask |= LVCF_SUBITEM;
			col_.iSubItem = idx;
		}

		// 列アイテムの挿入位置
		void index(int idx) {
			index_ = idx;
		}
		int index() const { return index_; }

	private:
		lv_column col_;
		string_type text_;
		int index_;
	};

}}}}// namespace lulib::win32api::window_detail::common_control
