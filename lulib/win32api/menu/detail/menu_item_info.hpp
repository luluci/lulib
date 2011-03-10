#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/detail/menu_traits.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu { namespace detail {

	template<typename Char = TCHAR>
	class basic_menu_item_info {
		typedef basic_menu_item_info<Char> self_type;

		// char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// Menu型特性
		typedef detail::menu_traits<Char> menu_traits;
		typedef typename menu_traits::menu_item_info_type menu_item_info_type;

	public:
		// ctor
		basic_menu_item_info() : mii_() {
			mii_.cbSize = sizeof(MENUITEMINFO);
		}
		// copy ctor
		basic_menu_item_info(self_type const& obj) : mii_(obj.mii_) {
		}
		basic_menu_item_info(self_type && obj) : mii_(obj.mii_) {
		}
		// copy op
		self_type& operator=(self_type const& obj) {
			mii_ = obj.mii_;
			return *this;
		}

		operator MENUITEMINFO*() {
			return &mii_;
		}

		// ID属性を付与
		self_type& id(std::size_t id) {
			mii_.fMask |= mask::id;
			mii_.wID = id;
			return *this;
		}

		// string属性を付与
		self_type& string(char_type const* str) {
			mii_.fMask |= mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str);
			return *this;
		}
		self_type& string(string_type const& str) {
			mii_.fMask |= mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str.c_str());
			return *this;
		}

		// submenuを付与
		self_type& submenu(HMENU hMenu) {
			mii_.fMask |= mask::submenu;
			mii_.hSubMenu = hMenu;
			return *this;
		}

		// separatorを付与
		self_type& separator() {
			mii_.fMask |= mask::ftype;
			mii_.fType = ftype::separator;
			return *this;
		}

		// stateを付与
		self_type& state(std::size_t s) {
			mii_.fMask |= mask::state;
			mii_.fState = s;
			return *this;
		}

	private:
		menu_item_info_type mii_;
	};

}}}}// namespace lulib::win32api::menu::detail
