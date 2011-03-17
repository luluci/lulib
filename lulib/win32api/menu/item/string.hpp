#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/win32api/menu/policy.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu { namespace item {

	template<typename Char = TCHAR>
	struct basic_string {
		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;
		// MenuItemInfo型
		typedef detail::basic_menu_item_info<Char> menu_item_info;

	public:
		basic_string(std::size_t id, char_type const* str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_string(std::size_t id, string_type const& str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_string(std::size_t id, string_type && str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_string(std::size_t idx, std::size_t id, char_type const* str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_string(std::size_t idx, std::size_t id, string_type const& str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_string(std::size_t idx, std::size_t id, string_type && str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}

		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& operator<<(basic_menu<C,T>&, basic_string<T>&&);

	private:
		menu_item_info mii_;
		std::size_t idx_;
	};

	// operator
	template<HMENU (WINAPI *C)(), typename Char>
	basic_menu<C,Char>& operator<<(basic_menu<C,Char>& menu, basic_string<Char> &&i) {
		// メニューアイテムを挿入
		if ( policy<Char>::insert_menu_item(menu, i.idx_, true, i.mii_) != TRUE ) {
			throw ra_error("failed to insert menu item.");
		}
		return menu;
	}

}}}}// namespace lulib::win32api::menu::item
