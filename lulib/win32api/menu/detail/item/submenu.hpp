#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu_fwd.hpp>
#include <lulib/win32api/menu/detail/policy.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu_detail { namespace item {

	template<typename Char = TCHAR>
	struct basic_submenu {
		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;
		// MenuItemInfo型
		typedef basic_menu_item_info<Char> menu_item_info;

	public:
		basic_submenu(std::size_t id, char_type const* str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_submenu(std::size_t id, string_type const& str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_submenu(std::size_t id, string_type && str) : mii_(), idx_(id) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_submenu(std::size_t idx, std::size_t id, char_type const* str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_submenu(std::size_t idx, std::size_t id, string_type const& str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}
		basic_submenu(std::size_t idx, std::size_t id, string_type && str) : mii_(), idx_(idx) {
			mii_.id(id);
			mii_.string(str);
		}

		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& operator<<(basic_menu<C,T>&, basic_submenu<T>&&);

	private:
		menu_item_info mii_;
		std::size_t idx_;
	};

	// operator
	template<HMENU (WINAPI *T)(), typename Char>
	basic_menu<T,Char>& operator<<(basic_menu<T,Char>& menu, basic_submenu<Char> &&i) {
		typedef basic_menu<T,Char> menu_type;

		// すでに同じIDのメニューアイテムが作られている場合
		if ( menu.c_.count( i.mii_.id() ) != 0 ) {
			throw ra_error("conflict menu item id.");
		}

		// サブメニューハンドラを作成
		auto result = menu.c_.insert(
			typename menu_type::container::value_type(
				i.mii_.id(),
				typename menu_type::submenu_ptr( new typename menu_type::submenu_type() )
			)
		);
		// サブメニューハンドラ作成に失敗
		if ( !result.second ) {
			throw ra_error("failed to insert submenu ptr.");
		}
		// サブメニューを登録
		i.mii_.submenu( *(result.first->second) );

		// メニューアイテムを挿入
		if ( policy<Char>::insert_menu_item(menu, i.idx_, true, i.mii_) != TRUE ) {
			throw ra_error("failed to insert menu item.");
		}
		return menu;
	}

}}}}// namespace lulib::win32api::menu_detail::item
