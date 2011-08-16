#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

#include <lulib/win32api/window/detail/window_base.hpp>
#include <lulib/win32api/window/detail/common_control/basic_list_view_op.hpp>
#include <lulib/win32api/window/detail/common_control/list_view_policy.hpp>
#include <lulib/win32api/window/detail/common_control/basic_list_view_column.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_list_view : public window_base<basic_list_view<Char>, basic_list_view<Char>, Char> {
			typedef basic_list_view<Char> self_type;
			// 基底クラス型
			typedef window_base<self_type, self_type, Char> base_type;
			// どのタイプのウィンドウか
			typedef self_type window_type;

			// ListViewポリシー
			typedef common_control::list_view_policy<Char> lv_policy;

		public:
			// ListView列型
			typedef common_control::basic_list_view_column<Char> column;
			// ListViewアイテム型
			typedef common_control::basic_list_view_item<Char> item;

		public:
			basic_list_view() : base_type(), lv_ex_style_(0) {
				// コモンコントロールの初期化
				init();
				// ListViewクラス名をセット
				this->class_name_ = lv_policy::wnd_class;
			}

		private:
			basic_list_view(self_type const&);
			self_type const& operator=(self_type const&);

		private:
			void init() {
				// コモンコントロールの初期化
				INITCOMMONCONTROLSEX icc;
				icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
				icc.dwICC  = ICC_LISTVIEW_CLASSES;
				InitCommonControlsEx(&icc);
			}

#ifndef __GNUC__
			template<typename D, typename W, typename C> friend class window_base;
#else
public:
#endif
			void on_create() {
				lv_ex_style_ = lv_policy::get_ex_style(*this);
			}

		/*
		public:
			struct lv_ex_style {
				enum enum_t {
					check_boxes      = LVS_EX_CHECKBOXES,     // チェックボックスをつけます
					full_row_select  = LVS_EX_FULLROWSELECT,  // １行選択を可能にします
					grid_lines       = LVS_EX_GRIDLINES,      // グリッド線を入れます
					header_drag_drop = LVS_EX_HEADERDRAGDROP, // ヘッダをドラッグ&ドロップ可能にします
				};
			};
		*/

		public:
			// swap
			void swap(self_type &obj) throw() {
				this->base_type::swap(obj);
				std::swap( lv_ex_style_, obj.lv_ex_style_ );
			}

		public:
			// friend関数
			// ListView Ex Style の操作
			// ウィンドウ作成後にしか操作は意味が無い
			typedef common_control::ex_style lv_ex_style;
			template<typename T>
			friend typename basic_list_view<T>::window_type&
			common_control::operator<<(basic_list_view<T>&, common_control::ex_style &&);
			// columnの操作
			template<typename T>
			friend typename basic_list_view<T>::window_type&
			common_control::operator<<(basic_list_view<T>&, common_control::basic_list_view_column<T> const&);
			// itemの操作
			template<typename T>
			friend typename basic_list_view<T>::window_type&
			common_control::operator<<(basic_list_view<T>&, common_control::basic_list_view_item<T> const&);

		private:
			// ListView Ex Style
			int lv_ex_style_;
		};

	}//namespace lulib::win32api::window_detail

	using window_detail::basic_list_view;

}}// namespace lulib::win32api
