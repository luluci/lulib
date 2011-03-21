#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/detail/policy.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu_detail {

	template<typename Char = TCHAR>
	class basic_menu_item_info {
		typedef basic_menu_item_info<Char> self_type;

		// char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// Menuポリシー
		typedef ::lulib::win32api::menu_detail::policy<Char> policy;
		typedef typename policy::menu_item_info_type menu_item_info_type;

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

		// IDを取得
		inline std::size_t id() { return mii_.wID; }
		// ID属性を付与
		self_type& id(std::size_t id) {
			mii_.fMask |= item_type::mask::id;
			mii_.wID = id;
			return *this;
		}

		// string属性を付与
		self_type& string(char_type const* str) {
			str_ = str;
			mii_.fMask |= item_type::mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str_.c_str());
			return *this;
		}
		self_type& string(string_type const& str) {
			str_ = str;
			mii_.fMask |= item_type::mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str_.c_str());
			return *this;
		}
		self_type& string(string_type && str) {
			str_ = std::move(str);
			mii_.fMask |= item_type::mask::string;
			mii_.dwTypeData = const_cast<char_type*>(str_.c_str());
			return *this;
		}

		// submenuを付与
		self_type& submenu(HMENU hMenu) {
			mii_.fMask |= item_type::mask::submenu;
			mii_.hSubMenu = hMenu;
			return *this;
		}

		// separatorを付与
		self_type& separator() {
			mii_.fMask |= item_type::mask::ftype;
			mii_.fType = item_type::ftype::separator;
			return *this;
		}

		// stateを付与
		self_type& state(std::size_t s) {
			mii_.fMask |= item_type::mask::state;
			mii_.fState = s;
			return *this;
		}

	private:
		menu_item_info_type mii_;
		string_type str_;
	};

}}}// namespace lulib::win32api::menu_detail
