#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <memory>
#include <functional>

#include <lulib/win32api/window/basic_window_class.hpp>
#include <lulib/win32api/window/detail/basic_window_op.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		struct HWND_deleter {
			typedef HWND pointer;
			void operator()(HWND hWnd) {
				::DestroyWindow(hWnd);
			}
		};

		template<typename Char = TCHAR>
		class basic_window {
			// self_type
			typedef basic_window<Char> self_type;
			// WindowClass
			typedef basic_window_class<Char> wnd_class;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

			// windowポリシー
			typedef ::lulib::win32api::window_detail::policy<Char> policy;

			// ウィンドウプロシージャコールバック
			typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> procedure_type;

			// HWNDハンドラ
			// これは共有しないよ！
#ifdef _MSC_VER
			typedef std::unique_ptr<HWND, HWND_deleter> wnd_ptr;
#else
			typedef std::unique_ptr< std::remove_pointer<HWND>::type, HWND_deleter> wnd_ptr;
#endif

			// Menu型
			typedef menu_detail::basic_menu< ::CreateMenu, Char > menu_type;
			typedef std::shared_ptr<menu_type> menu_ptr;

		public:
			basic_window()
				: wnd_ptr_(), ex_style_(), class_name_(), window_name_()
				, style_(WS_OVERLAPPEDWINDOW), x_(), y_(), w_(), h_(), hParent_()
				, menu_ptr_(), hInst_(), proc_()
			{
			}

			inline operator HWND() {
				return wnd_ptr_.get();
			}
			// 親ウィンドウハンドルを返す
			//inline HWND get_parent() { return hParent_; }

		private:
			basic_window(basic_window const&);
			self_type const& operator=(self_type const&);


		public:
			// ウィンドウ操作編
			inline void show() {
				::ShowWindow( wnd_ptr_.get(), SW_SHOW );
			}

		public:
			// リソース類の作成編

			// ウィンドウハンドルが::DestroyWindowで破棄されたのを受けて
			// ウィンドウハンドルを無効にする
			inline void disable() {
				// デリータを呼ばずにポインタを解放
				wnd_ptr_.release();
			}

			bool create() {
				// メニューハンドラが有効ならセットする
				HMENU hMenu = 0;
				if (menu_ptr_) hMenu = *menu_ptr_;

				// ウィンドウを作成
				HWND hWnd = policy::create_window(
					ex_style_,
					const_cast<char_type*>( class_name_.c_str() ),
					const_cast<char_type*>( window_name_.c_str() ),
					style_,
					x_, y_, w_, h_,
					hParent_,
					hMenu,
					hInst_,
					0
				);
				// 失敗なら
				if (hWnd == NULL) return false;

				// 成功なら、unique_ptrにセット
				wnd_ptr_.reset(hWnd);
				// GWLP_USERDATA にthisをセットする
				policy::set_window_long_ptr(
					wnd_ptr_.get(),
					GWLP_USERDATA,
					reinterpret_cast<LONG_PTR>(this)
				);

				return true;
			}

			// プロシージャの呼び出し
			inline LRESULT callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
				return proc_(hWnd, msg, wParam, lParam);
			}

		public:
			// friend関数
			// HINSTANCE の操作
			typedef attribute::instance_handle inst;
			typedef attribute::instance_handle instance;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::instance_handle &&);
			// ClassNameの操作
			typedef attribute::basic_window_class_name<Char> class_name;
			typedef attribute::basic_window_class_name<Char> cname;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::basic_window_class_name<T> &&);
			// 親ウィンドウハンドルの操作
			typedef attribute::basic_parent_window_handle<Char> parent;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::basic_parent_window_handle<T> &&);
			// メニューハンドルの操作
			typedef attribute::basic_menu_handle< ::CreateMenu, Char > menu;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::basic_menu_handle<C,T> &&);
			// ウィンドウプロシージャの操作
			typedef attribute::procedure procedure;
			typedef attribute::procedure proc;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::procedure &&);
			// Window Ex Style の操作
			typedef attribute::ex_style ex_style;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::ex_style &&);
			// Window Style の操作
			typedef attribute::style style;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::style &&);
			// タイトルバーテキスト の操作
			typedef attribute::basic_title<Char> title;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::basic_title<T> &&);
			// ウィンドウ位置の操作
			typedef attribute::position position;
			typedef attribute::position pos;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::position &&);
			// ウィンドウサイズの操作
			typedef attribute::size size;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::size &&);

		private:
			wnd_ptr wnd_ptr_;          // HWNDハンドラ
			std::size_t ex_style_;     // ウィンドウ拡張スタイル
			string_type class_name_;   // ウィンドウクラス名
			string_type window_name_;  // タイトルバーに表示されるテキスト
			std::size_t style_;        // ウィンドウスタイル
			int x_, y_, w_, h_;        // ウィンドウの座標とサイズ
			HWND hParent_;             // 親ウィンドウハンドル
			menu_ptr menu_ptr_;        // HMENUハンドラ : nullを許容
			HINSTANCE hInst_;          // HINSTANCE
			procedure_type proc_;      // ウィンドウプロシージャ
		};
	}//namespace lulib::win32api::window_detail

	using window_detail::basic_window;

}}// namespace lulib::win32api
