#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/menu/basic_menu.hpp>

#include <boost/optional.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// Menu型
	template<HMENU (WINAPI *C)(), typename Char = TCHAR>
	class basic_menu_handle {
		// メニュー型
		typedef lulib::win32api::basic_menu<C,Char> menu_type;
		typedef boost::optional<menu_type&> menu_opt;

	public:
		basic_menu_handle() : menu_() {}
		basic_menu_handle(menu_type &m) : menu_(m) {}

		inline menu_opt& operator()() { return menu_; }

	private:
		menu_opt menu_;
	};

}}}}// namespace lulib::win32api::window::attribute
