#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/menu/detail/policy.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace menu_detail { namespace item {

	struct separator {

	public:
		separator(std::size_t idx) : idx_(idx) {}

		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& operator<<(basic_menu<C,T>&, separator&&);

	private:
		std::size_t idx_;
	};

	// operator
	template<HMENU (WINAPI *T)(), typename Char>
	basic_menu<T,Char>& operator<<(basic_menu<T,Char>& menu, separator &&i) {
		// MenuItemInfo型
		typedef basic_menu_item_info<Char> menu_item_info;
		// MenuItemInfo
		menu_item_info mii;
		mii.separator();
		// メニューアイテムを挿入
		if ( policy<Char>::insert_menu_item(menu, i.idx_, true, mii) != TRUE ) {
			throw ra_error("failed to insert menu item.");
		}
		return menu;
	}

}}}}// namespace lulib::win32api::menu_detail::item
