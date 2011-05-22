#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace font_detail { namespace attribute {

	struct init {
		init(HWND hWnd) : hWnd_(hWnd) {}

		inline HWND operator()() { return hWnd_; }

	private:
		HWND hWnd_;
	};

}}}}// namespace lulib::win32api::window_detail::attribute
