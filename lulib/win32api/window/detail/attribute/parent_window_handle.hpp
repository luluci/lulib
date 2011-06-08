#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/window_base.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// HWND型
	template<typename Derived, typename Char = TCHAR>
	class basic_parent_window_handle {
		typedef window_base<Derived,Char> window;

	public:
		// デフォルトコンストラクタの場合、
		// トップレベルウィンドウ（親ウィンドウを持たないウィンドウ）になる
		basic_parent_window_handle() : hParent_(0) {}
		// 親ウィンドウハンドルを指定
		basic_parent_window_handle(HWND hParent) : hParent_(hParent) {}
		basic_parent_window_handle(window &wnd) : hParent_(wnd.parent()) {}

		inline HWND operator()() { return hParent_; }
		inline operator HWND() { return hParent_; }

	private:
		HWND hParent_;
	};

}}}}// namespace lulib::win32api::window::attribute
