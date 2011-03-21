#pragma once
#pragma warning(disable : 4819)

#include <windows.h>
//#include <commctrl.h>

#include <map>
#include <memory>
#include <type_traits>

#include <lulib/win32api/menu/detail/items.hpp>
#include <lulib/win32api/menu/detail/states.hpp>
#include <lulib/win32api/menu/detail/policy.hpp>
#include <lulib/win32api/menu/detail/menu_item_info.hpp>
#include <lulib/win32api/menu/detail/menu_item_type.hpp>
#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {

	namespace menu_detail {

		struct HMENU_deleter {
			typedef HMENU pointer;
			void operator()(HMENU hMenu) {
				if (hMenu) ::DestroyMenu(hMenu);
			}
		};

		// メニュークラス
		template<HMENU (WINAPI *create_menu)(), typename Char = TCHAR>
		class basic_menu {
			typedef basic_menu<create_menu, Char> self_type;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

			// Menuポリシー
			typedef ::lulib::win32api::menu_detail::policy<Char> policy;

			// MenuItemInfo型
			typedef basic_menu_item_info<Char> menu_item_info;

			// HMENUハンドラ
			// これは共有しないよ！
#ifdef _MSC_VER
			typedef std::unique_ptr<HMENU, HMENU_deleter> menu_ptr;
#else
			typedef std::unique_ptr< std::remove_pointer<HMENU>::type, HMENU_deleter> menu_ptr;
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
			submenu_ptr get_submenu(std::size_t idx) {
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

		public:
			// friend関数
			// メニューアイテム
			// 文字列メニューアイテム
			typedef item::basic_string<Char> string;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::basic_string<T> &&);
			// サブメニューアイテム
			typedef item::basic_submenu<Char> submenu;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::basic_submenu<T> &&);
			// セパレータアイテム
			typedef item::separator separator;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& item::operator<<(basic_menu<C,T>&, item::separator &&);

			// 状態
			// 通常
			typedef state::enable enable;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::enable &&);
			// 無効
			typedef state::disabled disabled;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::disabled &&);
			// チェックマーク
			typedef state::checked checked;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::checked &&);
			// ハイライト(選択状態になってる)
			typedef state::hilite hilite;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::hilite &&);
			// デフォルト(太字になってる)
			typedef state::defitem defitem;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::defitem &&);
			// 複合状態
			typedef item_type::state states;
			typedef state::multi multi;
			template<HMENU (WINAPI *C)(), typename T>
			friend basic_menu<C,T>& state::operator<<(basic_menu<C,T>&, state::multi &&);

		private:
			// メニューハンドラ
			// HMENU create_menu() によってメニュー or サブメニュー
			menu_ptr menu_ptr_;
			// サブメニューコンテナ
			container c_;

		};
	}// namespace lulib::win32api::menu_detail

	using menu_detail::basic_menu;

}}// namespace lulib::win32api
