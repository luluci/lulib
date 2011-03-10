#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/basic_window.hpp>
#include <lulib/win32api/window/window_traits.hpp>

#include <boost/mpl/print.hpp>

namespace lulib { namespace win32api { namespace window {

	template<typename Char>
	LRESULT CALLBACK procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		//boost::mpl::print< window::basic_window<char> > abc;
		// window型
		typedef window::basic_window<Char> window_type;
		// Window型特性
		typedef window_traits<Char> window_traits;

		// USERDATAを取得
		LONG_PTR ptr = window_traits::get_window_long_ptr(hWnd, GWLP_USERDATA);

		// 0ならDefWindowProcにまかせる
		if (ptr == 0) return window_traits::def_window_proc(hWnd, msg, wParam, lParam);

		// 値があれば、一部のメッセージを処理
		// ポインタを取得
		window_type *self = reinterpret_cast<window_type*>(ptr);
		switch (msg) {
			// ウィンドウハンドルがちゃんと破棄された
			case WM_DESTROY: {
				// ウィンドウハンドルを無効にする
				self->disable();
				break;
			}
		}
		// その他の処理はまかせる
		return self->callback(hWnd, msg, wParam, lParam);
	}

}}}// namespace lulib::win32api::window
