#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// Windowポリシー
	template<typename Char>
	struct static_text_policy;

	template<>
	struct static_text_policy<char> {
		// WindowClassName
		static char const* wnd_class;

		// default procedure
		typedef LRESULT (CALLBACK* wnd_proc)(HWND,UINT,WPARAM,LPARAM);
		static wnd_proc default_procedure;
	};
	char const* static_text_policy<char>::wnd_class = WC_STATICA;
	static_text_policy<char>::wnd_proc static_text_policy<char>::default_procedure = 0;

	template<>
	struct static_text_policy<wchar_t> {
		// WindowClassName
		static wchar_t const* wnd_class;

		// default procedure
		typedef LRESULT (CALLBACK* wnd_proc)(HWND,UINT,WPARAM,LPARAM);
		static wnd_proc default_procedure;
	};
	wchar_t const* static_text_policy<wchar_t>::wnd_class = WC_STATICW;
	static_text_policy<wchar_t>::wnd_proc static_text_policy<wchar_t>::default_procedure = 0;

}}}}// namespace lulib::win32api::window_detail::common_control
