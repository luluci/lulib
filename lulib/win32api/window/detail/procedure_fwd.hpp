#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace window_detail {

	// forward
	template<typename Char>
	LRESULT CALLBACK procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

}}}// namespace lulib::win32api::window_detail
