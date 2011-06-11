#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

#include <memory>
#include <type_traits>
#include <cstdint>

#include <lulib/win32api/window/detail/window_base.hpp>
#include <lulib/win32api/window/detail/common_control/static_text_policy.hpp>
#include <lulib/win32api/window/detail/common_control/basic_static_text_op.hpp>
#include <lulib/win32api/window/detail/common_control/subclass.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_static_text : public window_base<basic_static_text<Char>, Char> {
			typedef basic_static_text<Char> self_type;
			// 基底クラス型
			typedef window_base<basic_static_text<Char>, Char> base_type;

			// policy
			typedef common_control::static_text_policy<Char> policy;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

			// WNDPROCハンドラ
			//typedef common_control::WNDPROC_handler<Char> WNDPROC_handler;
			//typedef std::unique_ptr< std::remove_pointer<WNDPROC>::type, WNDPROC_handler> proc_handler;
			typedef common_control::WNDPROC_handler<Char> proc_handler;

		public:
			basic_static_text() : base_type(), proc_handler_() {
				// ListViewクラス名をセット
				this->class_name_ = policy::wnd_class;
			}
			basic_static_text(
				char_type const* text, std::uint16_t id = 0, std::size_t style = (WS_VISIBLE | WS_CHILD),
				HINSTANCE hInst = 0, HWND hParent = 0, int x = 0, int y = 0, int w = 0, int h = 0
			) : base_type(), proc_handler_() {
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
			template<typename T, typename U> friend class window_base;
#else
public:
#endif

			// CreateWindowEx(), SetWindowLongPtr(hWnd,GWLP_USERDATA,this)
			// の実行後に呼ばれる
			void on_create() {
				// プロシージャがセットされていたらサブクラス化
				if (this->proc_) {
					common_control::subclassing<self_type,Char>(*this);
				}
			}

		public:
			// プロシージャの呼び出し
			inline LRESULT callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
				// プロシージャがセットされているなら実行
				if (this->proc_) return this->proc_(hWnd, msg, wParam, lParam);
				// プロシージャがセットされていないなら、DefWindowProcにまかせる
				else return policy::default_procedure(hWnd, msg, wParam, lParam);
			}

			// コントロールのデフォルトプロシージャ
			inline LRESULT default_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
				if (proc_handler_) {
					return proc_handler_.get()(hWnd,msg,wParam,lParam);
				}
				return 0;
			}

		public:
			// swap
			void swap(self_type &obj) throw() {
				this->base_type::swap(obj);
				std::swap( proc_handler_, obj.proc_handler_ );
			}

		public:
			// friend関数
			// subclassing
			typedef common_control::subclass subclass;
			template<typename T,typename U>
			friend void common_control::subclassing(T&);
			template<typename T>
			friend basic_static_text<T>& common_control::operator<<(
				basic_static_text<T>&, common_control::subclass&&);

		private:
			// コモンコントロールデフォルトプロシージャハンドラ
			proc_handler proc_handler_;
		};

	}//namespace lulib::win32api::window_detail

	using window_detail::basic_static_text;

}}// namespace lulib::win32api
