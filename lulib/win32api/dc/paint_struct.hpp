#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/dc/select_object.hpp>

namespace lulib { namespace win32api {

	// PAINTSTRUCTƒ‰ƒbƒp
	class paint_struct {
		// self_type
		typedef paint_struct self_type;

		// SelectObject Handler
		typedef dc_detail::select_object<HFONT> select_font;

	public:
		paint_struct(HWND hWnd)
		: hWnd_(hWnd), ps_()
		, select_font_()
		{
			::BeginPaint( hWnd_, &ps_ );
			select_font_ = ps_.hdc;
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

		self_type& operator<<(HFONT hFont) {
			select_font_ = hFont;
			return *this;
		}

	private:
		HWND hWnd_;
		PAINTSTRUCT ps_;
		select_font select_font_;
	};

}}// namespace lulib::win32api
