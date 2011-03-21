#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/menu/detail/policy.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace menu_detail { namespace state {

	struct disabled {

	public:
		disabled(std::size_t id) : id_(id) {}

		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& operator<<(basic_menu<C,T>&, disabled&&);

	private:
		std::size_t id_;
	};

	// operator
	template<HMENU (WINAPI *T)(), typename Char>
	basic_menu<T,Char>& operator<<(basic_menu<T,Char>& menu, disabled &&s) {
		// MenuItemInfo型
		typedef basic_menu_item_info<Char> menu_item_info;
		// MenuItemInfo
		menu_item_info mii;
		mii.state( item_type::state::disabled );
		// メニューアイテムを挿入
		if ( policy<Char>::set_menu_item_info(menu, s.id_, FALSE, mii) != TRUE ) {
			throw ra_error("failed to insert menu item.");
		}
		return menu;
	}

}}}}// namespace lulib::win32api::menu_detail::state
