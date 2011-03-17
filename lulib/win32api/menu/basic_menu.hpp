#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
//#include <commctrl.h>

#include <map>
#include <memory>
#include <type_traits>

#include <lulib/win32api/menu/policy.hpp>
#include <lulib/win32api/menu/items_fwd.hpp>
#include <lulib/win32api/menu/states_fwd.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace menu {

	namespace detail {
		struct HMENU_deleter {
			typedef HMENU pointer;
			void operator()(HMENU hMenu) {
				if (hMenu) ::DestroyMenu(hMenu);
			}
		};
	}

	// メニュークラス
	template<HMENU (WINAPI *create_menu)(), typename Char = TCHAR>
	class basic_menu {
		typedef basic_menu<create_menu, Char> self_type;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// Menuポリシー
		typedef ::lulib::win32api::menu::policy<Char> policy;

		// MenuItemInfo型
		typedef detail::basic_menu_item_info<Char> menu_item_info;

		// HMENUハンドラ
		// これは共有しないよ！
#ifdef _MSC_VER
		typedef std::unique_ptr<HMENU, detail::HMENU_deleter> menu_ptr;
#else
		typedef std::unique_ptr< std::remove_pointer<HMENU>::type, detail::HMENU_deleter> menu_ptr;
#endif

		// サブメニュー
		typedef basic_menu< ::CreatePopupMenu > submenu_type;
		typedef std::shared_ptr< submenu_type > submenu_ptr;
		// サブメニューコンテナ
		// MIIM_SUBMENUが有効なときは、そのHMENUは管理しなきゃならない
		// メニューIDとその箇所のメニューハンドラ
		typedef std::map< std::size_t, submenu_ptr > container;

	public:
		basic_menu(void)
			: menu_ptr_( create_menu() )
		{
			if (!menu_ptr_) {
				throw ra_error("faild to CreateMenu().");
			}
		}
		basic_menu(self_type &&obj)
			: menu_ptr_( std::move(obj.menu_ptr_) ), c_( std::move(obj.c_) )
		{
		}
		self_type& operator=(self_type &&obj) {
			menu_ptr_ = std::move(obj.menu_ptr_);
			c_ = std::move(obj.c_);
			return *this;
		}
	private:
		basic_menu(self_type const& obj);
		self_type& operator=(self_type const& obj);

	public:
		inline operator HMENU() { return menu_ptr_.get(); }

		// サブメニューハンドラの取得
		submenu_ptr submenu(std::size_t idx) {
			auto it = c_.find(idx);
			if ( it != c_.end() ) {
				return it->second;
			}
			else {
				return submenu_ptr();
			}
		}

		// x, y: スクリーン座標( ClientToScreen() )
		bool popup(HWND hWnd, int x, int y) {
			std::uint32_t flag = TPM_LEFTALIGN | TPM_TOPALIGN;
			return (::TrackPopupMenuEx(menu_ptr_.get(), flag, x, y, hWnd, NULL) == TRUE);
		}

		// stateを使う
		//typedef detail::state state;
		// 対象メニューアイテムの状態を変更
		/*
		bool set_state(std::size_t id, std::size_t s) {
			// idを識別子として使用
			return set_state(id, FALSE, s);
		}
		bool set_state(std::size_t id, BOOL id_is_position, std::size_t s) {
			menu_item_info mii;
			mii.state(s);
			return ( policy::set_menu_item_info(menu_ptr_.get(), id, id_is_position, mii) == TRUE );
		}
		// enableにする
		bool set_enable(std::size_t id) {
			return set_state(id, FALSE, state::enable);
		}
		bool set_disabled(std::size_t id) {
			return set_state(id, FALSE, state::disabled);
		}
		bool set_checked(std::size_t id) {
			return set_state(id, FALSE, state::checked);
		}
		bool set_hilite(std::size_t id) {
			return set_state(id, FALSE, state::hilite);
		}
		bool set_defitem(std::size_t id) {
			return set_state(id, FALSE, state::defitem);
		}
		*/

	public:
		// friend関数
		// メニューアイテム
		// 文字列メニューアイテム
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::basic_string<T> &&);
		// サブメニューアイテム
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::basic_submenu<T> &&);
		// セパレータアイテム
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::separator &&);

		// 状態
		// 通常
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::enable &&);
		// 無効
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::disabled &&);
		// チェックマーク
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::checked &&);
		// ハイライト(選択状態になってる)
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::hilite &&);
		// デフォルト(太字になってる)
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::defitem &&);
		// 複合状態
		template<HMENU (WINAPI *C)(), typename T>
		friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, state::multi &&);

	private:
		// メニューハンドラ
		// HMENU create_menu() によってメニュー or サブメニュー
		menu_ptr menu_ptr_;
		// サブメニューコンテナ
		container c_;

	};

}}}// namespace lulib::win32api::menu
