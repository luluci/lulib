#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/win32api/menu/policy.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace menu { namespace state {

	struct checked {

	public:
		checked(std::size_t id) : id_(id) {}

		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& operator<<(basic_menu<C,T>&, checked&&);

	private:
		std::size_t id_;
	};

	// operator
	template<HMENU (WINAPI *T)(), typename Char>
	basic_menu<T,Char>& operator<<(basic_menu<T,Char>& menu, checked &&s) {
		// MenuItemInfo型
		typedef detail::basic_menu_item_info<Char> menu_item_info;
		// MenuItemInfo
		menu_item_info mii;
		mii.state( detail::state::checked );
		// メニューアイテムを挿入
		if ( policy<Char>::set_menu_item_info(menu, s.id_, FALSE, mii) != TRUE ) {
			throw ra_error("failed to insert menu item.");
		}
		return menu;
	}

}}}}// namespace lulib::win32api::menu::state
