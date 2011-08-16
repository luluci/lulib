#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
#include <commctrl.h>

#include <lulib/win32api/window/detail/common_control/subclass.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// Windowポリシー
	template<typename Char>
	struct static_text_policy;

	template<>
	struct static_text_policy<char> {
		// WindowClassName
		static char const* wnd_class;
	};
	char const* static_text_policy<char>::wnd_class = WC_STATICA;

	template<>
	struct static_text_policy<wchar_t> {
		// WindowClassName
		static wchar_t const* wnd_class;
	};
	wchar_t const* static_text_policy<wchar_t>::wnd_class = WC_STATICW;

}}}}// namespace lulib::win32api::window_detail::common_control
