#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/window_base.hpp>

namespace lulib { namespace win32api {

	// PAINTSTRUCTƒ‰ƒbƒp
	class paint_struct {
		// self_type
		typedef paint_struct self_type;

	public:
		paint_struct(HWND hWnd) : hWnd_(hWnd), ps_() {
			::BeginPaint( hWnd_, &ps_ );
		}
		~paint_struct() {
			::EndPaint( hWnd_, &ps_ );
		}
	private:
		paint_struct(self_type const&);
		self_type const& operator=(self_type const&);

	public:
		operator PAINTSTRUCT&() { return ps_; }
		operator HDC&() { return ps_.hdc; }

	private:
		HWND hWnd_;
		PAINTSTRUCT ps_;
	};

}}// namespace lulib::win32api
