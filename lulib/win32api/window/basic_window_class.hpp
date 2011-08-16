#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/detail/procedure.hpp>
#include <lulib/win32api/window/detail/window_policy.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_window_class {
			// self_type
			typedef basic_window_class<Char> self_type;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

			// windowポリシー
			typedef window_policy<Char> policy;
			typedef typename policy::wnd_class wnd_class;

			// window
			typedef basic_window<Char> window_type;

		public:
			basic_window_class() : wc_(), class_name_() {
				init();
			}

			inline ATOM atom() { return atom_; }

			bool register_class() {
				atom_ = policy::register_class(&wc_);
				return ( atom_ != 0 );
			}

			// Default
			void init() {
				wc_.cbSize        = sizeof(wnd_class);
				wc_.style         = styles::h_redraw | styles::v_redraw;
				WNDPROC proc = procedure<window_type,char_type>;
				wc_.lpfnWndProc   = proc;
				wc_.hCursor       = LoadCursor(NULL,IDC_ARROW);
				wc_.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
			}

			// hInstance をセット
			void instance(HINSTANCE hInst) {
				wc_.hInstance = hInst;
			}

			// class name を返す
			string_type const& class_name() const {
				return class_name_;
			}
			// class name をセット
			void class_name(char_type const* classname) {
				class_name_ = classname;
				wc_.lpszClassName = const_cast<char_type*>( class_name_.c_str() );
			}
			void class_name(string_type const& classname) {
				class_name_ = classname;
				wc_.lpszClassName = const_cast<char_type*>( class_name_.c_str() );
			}

			struct styles {
				enum enum_t {
					byte_align_client = CS_BYTEALIGNCLIENT,
					byte_align_window = CS_BYTEALIGNWINDOW,
					class_dc          = CS_CLASSDC,
					double_clicks     = CS_DBLCLKS,
					h_redraw          = CS_HREDRAW,
					no_close          = CS_NOCLOSE,
					own_dc            = CS_OWNDC,
					parent_dc         = CS_PARENTDC,
					save_bitmap       = CS_SAVEBITS,
					v_redraw          = CS_VREDRAW,
				};
			};
			// styleをセット
			void style(std::size_t s) {
				wc_.style = s;
			}

			// 背景色
			void background(HBRUSH hBrush) {
				wc_.hbrBackground = hBrush;
			}

			// アイコン
			void icon(HICON hIcon) {
				wc_.hIcon = hIcon;
			}
			void icon_small(HICON hIcon) {
				wc_.hIconSm = hIcon;
			}

			// カーソル
			void cursor(HCURSOR hCursor) {
				wc_.hCursor = hCursor;
			}

		private:
			wnd_class wc_;
			string_type class_name_;
			ATOM atom_;  // 登録したWindowClassを一意に識別するアトム
		};

	}// namespace lulib::win32api::window_detail

	using window_detail::basic_window_class;

}}// namespace lulib::win32api
