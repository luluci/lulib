#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/menu/basic_menu.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// Menu型
	template<HMENU (WINAPI *C)(), typename Char = TCHAR>
	class basic_menu_handle {
		// メニュー型
		typedef lulib::win32api::basic_menu<C,Char> menu;
		typedef std::shared_ptr<menu> menu_ptr;

	public:
		basic_menu_handle(menu_ptr menu) : menu_(menu) {}

		inline operator menu_ptr() { return menu_; }

	private:
		menu_ptr menu_;
	};

}}}}// namespace lulib::win32api::window::attribute
