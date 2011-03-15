#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <memory>
#include <functional>

#include <lulib/win32api/window/basic_window_class.hpp>
#include <lulib/win32api/window/attributes_fwd.hpp>
#include <lulib/win32api/window/policy.hpp>
#include <lulib/win32api/menu_fwd.hpp>
#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window {

	namespace detail {
		struct HWND_deleter {
			typedef HWND pointer;
			void operator()(HWND hWnd) {
				::DestroyWindow(hWnd);
			}
		};
	}

	template<typename Char = TCHAR>
	class basic_window {
		// self_type
		typedef basic_window<Char> self_type;
		// WindowClass
		typedef basic_window_class<Char> wnd_class;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// windowポリシー
		typedef window::policy<TCHAR> policy;

		// ウィンドウプロシージャコールバック
		typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> procedure_type;

		// HWNDハンドラ
		// これは共有しないよ！
#ifdef _MSC_VER
		typedef std::unique_ptr<HWND, detail::HWND_deleter> wnd_ptr;
#else
		typedef std::unique_ptr< std::remove_pointer<HWND>::type, detail::HWND_deleter> wnd_ptr;
#endif

		// Menu型
		typedef menu::basic_menu< ::CreateMenu, Char > menu_type;
		typedef std::shared_ptr<menu_type> menu_ptr;

	public:
		basic_window()
			: wnd_ptr_(), ex_style_(), class_name_(), window_name_()
			, style_(WS_OVERLAPPEDWINDOW)
		{
		}

	private:
		basic_window(basic_window const&);
		self_type const& operator=(self_type const&);


	public:
		// ウィンドウ操作編
		inline void show() {
			::ShowWindow( wnd_ptr_.get(), SW_SHOW );
		}

	public:
		// リソース類の作成編

		// ウィンドウハンドルが::DestroyWindowで破棄されたのを受けて
		// ウィンドウハンドルを無効にする
		inline void disable() {
			// デリータを呼ばずにポインタを解放
			wnd_ptr_.release();
		}

		bool create() {
			// メニューハンドラが有効ならセットする
			HMENU hMenu = 0;
			if (menu_ptr_) hMenu = *menu_ptr_;

			// ウィンドウを作成
			HWND hWnd = policy::create_window(
				ex_style_,
				const_cast<char_type*>( class_name_.c_str() ),
				const_cast<char_type*>( window_name_.c_str() ),
				style_,
				pos_.left, pos_.top, (pos_.right - pos_.left), (pos_.bottom - pos_.top),
				hParent_,
				hMenu,
				hInst_,
				0
			);
			// 失敗なら
			if (hWnd == NULL) return false;

			// 成功なら、unique_ptrにセット
			wnd_ptr_.reset(hWnd);
			// GWLP_USERDATA にthisをセットする
			policy::set_window_long_ptr(
				wnd_ptr_.get(),
				GWLP_USERDATA,
				reinterpret_cast<LONG_PTR>(this)
			);

			return true;
		}

		// ウィンドウの名前
		inline void window_name(char_type const* str) { window_name_ = str; }
		inline void window_name(string_type const& str) { window_name_ = str; }

		// ウィンドウクラスのセット
		inline void class_name(char_type const* str) { class_name_ = str; }
		inline void class_name(string_type const& str) { class_name_ = str; }
		inline void class_name(wnd_class const& wclass) { class_name_ = wclass.class_name(); }

		// hInstanceのセット
		inline void instance(HINSTANCE hInst) { hInst_ = hInst; }

		// 親ウィンドウハンドルのセット
		inline void parent(HWND hParent) { hParent_ = hParent; }

		// プロシージャの呼び出し
		inline LRESULT callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			return proc_(hWnd, msg, wParam, lParam);
		}
		// プロシージャの指定
		// basic_window_class<Char>で登録したウィンドウクラスなら自動でコールバックされる
		template<typename Func>
		inline void procedure(Func const& func) { proc_ = func; }
		template<typename Func>
		inline void procedure(Func &&func) { proc_ = std::move(func); }

		// Menu型のセット
		inline void menu(menu_ptr const& p) {
			menu_ptr_ = p;
		}
		inline void menu(menu_ptr     && p) {
			menu_ptr_ = std::move(p);
		}

		// 座標のセット
		inline void rect(RECT const& rc) { pos_ = rc; }
		inline void rect(int x, int y, int w, int h) {
			pos_.left = x; pos_.top = y;
			pos_.right = x + w; pos_.bottom = y + h;
		}

	public:
		// friend関数
		template<typename T> friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::ex_style &&);
		template<typename T> friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::style &&);

	private:
		wnd_ptr wnd_ptr_;
		std::size_t ex_style_;
		string_type class_name_;
		string_type window_name_;
		std::size_t style_;
		RECT pos_;
		HWND hParent_;
		// Menu型のshared_ptrを持つ
		menu_ptr menu_ptr_;
		HINSTANCE hInst_;
		procedure_type proc_;
	};

}}}// namespace lulib::win32api::window
