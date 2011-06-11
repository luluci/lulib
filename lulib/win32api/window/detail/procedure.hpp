#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/policy.hpp>

namespace lulib { namespace win32api { namespace window_detail {

	template<typename Window, typename Char>
	LRESULT CALLBACK procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		//boost::mpl::print< window::basic_window<char> > abc;
		// window型
		typedef Window window_type;
		// Windowポリシー
		typedef policy<Char> policy;

		// USERDATAを取得
		LONG_PTR ptr = policy::get_window_long_ptr(hWnd, GWLP_USERDATA);

		// 0ならDefWindowProcにまかせる
		if (ptr == 0) return policy::default_procedure(hWnd, msg, wParam, lParam);

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

}}}// namespace lulib::win32api::window_detail
