#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/basic_window.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// HWND型
	template<typename Char = TCHAR>
	class basic_parent_window_handle {
		typedef basic_window<Char> window;

	public:
		basic_parent_window_handle(HWND hParent) : hParent_(hParent) {}
		basic_parent_window_handle(window &wnd) : hParent_(wnd.parent()) {}

		inline operator HWND() { return hParent_; }

	private:
		HWND hParent_;
	};

}}}}// namespace lulib::win32api::window::attribute
