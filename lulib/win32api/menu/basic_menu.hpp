#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
//#include <commctrl.h>

#include <map>
#include <memory>
#include <type_traits>

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
	template<HMENU (WINAPI *create_menu)()>
	class basic_menu {
		typedef basic_menu<create_menu> self_type;

		// char型
		typedef lulib::char_traits<TCHAR> char_traits;
		typedef char_traits::char_type   char_type;
		typedef char_traits::string_type string_type;

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

		// 通常のテキストで表示されるテキストの挿入
		bool insert_item(std::size_t id, char_type const* string) {
			return insert_item(id, id, string);
		}
		bool insert_item(std::size_t idx, std::size_t id, char_type const* string) {
			detail::menu_item_info mii;
			mii.id(id);
			mii.string(string);
			// メニューアイテムを挿入
			return ( ::InsertMenuItem(menu_ptr_.get(), idx, true, mii) == TRUE );
		}

		// セパレータの挿入
		bool insert_separator(std::size_t idx) {
			detail::menu_item_info mii;
			mii.separator();
			// セパレータを追加
			return ( ::InsertMenuItem(menu_ptr_.get(), idx, true, mii) == TRUE );
		}

		// サブメニューアイテムを挿入
		bool insert_submenu(std::size_t id, char_type const* string) {
			return insert_submenu(id, id, string);
		}
		bool insert_submenu(std::size_t idx, std::size_t id, char_type const* string) {
			detail::menu_item_info mii;
			mii.id(id);
			mii.string(string);

			// サブメニューハンドラを作成
			auto result = c_.insert(
				typename container::value_type(id, submenu_ptr( new submenu_type() ))
			);
			mii.submenu( *(result.first->second) );

			// メニューアイテムを挿入
			return ( ::InsertMenuItem(menu_ptr_.get(), idx, true, mii) == TRUE );
		}

		// stateを使う
		typedef detail::state state;
		// 対象メニューアイテムの状態を変更
		bool set_state(std::size_t id, std::size_t s) {
			// idを識別子として使用
			return set_state(id, FALSE, s);
		}
		bool set_state(std::size_t id, BOOL id_is_position, std::size_t s) {
			detail::menu_item_info mii;
			mii.state(s);
			return ( ::SetMenuItemInfo(menu_ptr_.get(), id, id_is_position, mii) == TRUE );
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

	private:
		// メニューハンドラ
		// HMENU create_menu() によってメニュー or サブメニュー
		menu_ptr menu_ptr_;
		// サブメニューコンテナ
		container c_;

	};

}}}// namespace lulib::win32api::menu
