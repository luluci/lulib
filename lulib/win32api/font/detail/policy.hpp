#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace font_detail {

	// char 特性
	template<typename Char>
	struct policy;

	template<>
	struct policy<char> {
		// LOGFONT
		typedef LOGFONTA log_font;

		// printf format
		static char const* format;

		// ::CreateFontIndirect
		inline static HFONT create_font_indirect(log_font const* lf) {
			return ::CreateFontIndirectA(lf);
		}

	};
	char const* policy<char>::format = "%s";

	template<>
	struct policy<wchar_t> {
		// LOGFONT
		typedef LOGFONTW log_font;

		// printf format
		static wchar_t const* format;

		// ::CreateFontIndirect
		inline static HFONT create_font_indirect(log_font const* lf) {
			return ::CreateFontIndirectW(lf);
		}

	};
	wchar_t const* policy<wchar_t>::format = L"%s";

}}}// namespace lulib::win32api::font
