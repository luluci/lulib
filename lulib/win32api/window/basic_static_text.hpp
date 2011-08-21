#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

#include <memory>
#include <type_traits>
#include <cstdint>

#include <lulib/win32api/window/detail/common_control/common_control_base.hpp>
#include <lulib/win32api/window/detail/common_control/static_text_policy.hpp>
//#include <lulib/win32api/window/detail/common_control/basic_static_text_op.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_static_text
		: public common_control::common_control_base<basic_static_text<Char>, basic_static_text<Char>, Char>
		{
			typedef basic_static_text<Char> self_type;
			// 基底クラス型
			typedef common_control::common_control_base<self_type, self_type, Char> base_type;
			// 最上位派生クラス
			// 大元のクラス
			// どのタイプのウィンドウか
			typedef self_type window_type;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

		public:
			// policy
			typedef common_control::static_text_policy<Char> policy;

		public:
			basic_static_text() : base_type() {
				// StaticTextクラス名をセット
				this->class_name_ = policy::wnd_class;
			}
			basic_static_text(
				char_type const* text, std::uint16_t id = 0, std::size_t style = (WS_VISIBLE | WS_CHILD),
				HINSTANCE hInst = 0, HWND hParent = 0, int x = 0, int y = 0, int w = 0, int h = 0
			) : base_type() {
				// ListViewクラス名をセット
				this->class_name_ = policy::wnd_class;
				// text
				this->window_name_ = text;
				// window id
				this->hMenu_ = reinterpret_cast<HMENU>(id);
				// instance handler/parent window
				this->hInst_ = hInst;
				this->hParent_ = hParent;
				// Style
				this->style_ = style;
				// position/size
				this->x_ = x;
				this->y_ = y;
				this->w_ = w;
				this->h_ = h;

				// CreateWindow
				this->create();
			}

		private:
			basic_static_text(self_type const&);
			self_type const& operator=(self_type const&);

#ifndef __GNUC__
			template<typename D, typename W, typename C> friend class window_base;
#else
public:
#endif

			// CreateWindowEx(), SetWindowLongPtr(hWnd,GWLP_USERDATA,this)
			// の実行後に呼ばれる
			void on_create() {
			}

		public:
			// swap
			void swap(self_type &obj) throw() {
				this->base_type::swap(obj);
			}

		};

	}//namespace lulib::win32api::window_detail

	using window_detail::basic_static_text;

}}// namespace lulib::win32api
