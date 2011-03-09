#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu { namespace detail {

	class menu_item_info {
		// char型
		typedef lulib::char_traits<TCHAR> char_traits;
		typedef char_traits::char_type   char_type;
		typedef char_traits::string_type string_type;

	public:
		// ctor
		menu_item_info() : mii_() {
			mii_.cbSize = sizeof(MENUITEMINFO);
		}
		// copy ctor
		menu_item_info(menu_item_info const& obj) : mii_(obj.mii_) {
		}
		menu_item_info(menu_item_info && obj) : mii_(obj.mii_) {
		}
		// copy op
		menu_item_info& operator=(menu_item_info const& obj) {
			mii_ = obj.mii_;
		}

		operator MENUITEMINFO*() {
			return &mii_;
		}

		// ID属性を付与
		menu_item_info& id(std::size_t id) {
			mii_.fMask |= mask::id;
			mii_.wID = id;
		}

		// string属性を付与
		menu_item_info& string(char_type const* str) {
			mii_.fMask |= mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str);
		}
		menu_item_info& string(string_type const& str) {
			mii_.fMask |= mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str.c_str());
		}

		// submenuを付与
		menu_item_info& submenu(HMENU hMenu) {
			mii_.fMask |= mask::submenu;
			mii_.hSubMenu = hMenu;
		}

		// separatorを付与
		menu_item_info& separator() {
			mii_.fMask |= mask::ftype;
			mii_.fType = ftype::separator;
		}

		// stateを付与
		menu_item_info& state(std::size_t s) {
			mii_.fMask |= mask::state;
			mii_.fState = s;
		}

	private:
		MENUITEMINFO mii_;
	};

}}}}// namespace lulib::win32api::menu::detail
