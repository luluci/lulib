#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <memory>
#include <functional>
#include <type_traits>


#include <lulib/win32api/window/detail/window_base_op.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

#include <lulib/utility.hpp>

// struct lulib::type_traits::has_on_create を生成
#include <lulib/type_traits/has_func.hpp>
LULIB_GEN_HAS_FUNC(on_create)

namespace lulib { namespace win32api { namespace window_detail {

	struct HWND_deleter {
		typedef HWND pointer;
		void operator()(HWND hWnd) {
			::DestroyWindow(hWnd);
		}
	};

	// HWNDが子ウィンドウであるとき、
	// 親ウィンドウをNULLにセットして関連付けを外す
	// hParentとhWndが親子関係にあるとき
	//   hParentが破棄された: hWndも自動で削除、無効オブジェクトになる
	//   hWndが破棄された   : 破棄の前にhParentを親ウィンドウでなくし、
	//                        hParentが無効オブジェクトを破棄することがないようにする
	struct HWND_handler {
		typedef HWND pointer;
		void operator()(HWND hWnd) {
			// 親ウィンドウが存在する場合
			if ( ::GetAncestor(hWnd, GA_PARENT) != NULL ) {
				// 親ウィンドウとの関係を断つ
				::SetParent(hWnd, 0);
			}
		}
	};

	template<typename Derived, typename Char = TCHAR>
	class window_base {
		// self_type
		typedef window_base<Derived, Char> self_type;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// windowポリシー
		typedef ::lulib::win32api::window_detail::policy<Char> policy;
		// policyは派生クラスでhidingされるので、一応
		typedef ::lulib::win32api::window_detail::policy<Char> wnd_policy;

		// HWNDハンドラ
		// HWNDが生きたままwindow_baseが破棄されるとき、デリータが呼ばれる
		// これは共有しないよ！
#ifdef _MSC_VER
		typedef std::unique_ptr<HWND, HWND_deleter> wnd_ptr;
#else
		typedef std::unique_ptr< std::remove_pointer<HWND>::type, HWND_deleter> wnd_ptr;
#endif

		// 自分が子ウィンドウの場合、破棄されるときには親ウィンドウとの関連を外す
		typedef std::unique_ptr< std::remove_pointer<HWND>::type, HWND_handler> child_handler;

		// ウィンドウプロシージャコールバック
		typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> procedure_type;

	public:
		window_base()
			: wnd_ptr_(), ex_style_(0), class_name_(), window_name_()
			, style_(WS_OVERLAPPEDWINDOW), x_(0), y_(0), w_(0), h_(0), hParent_(NULL)
			, hMenu_(NULL), hInst_(NULL), proc_(), child_handler_()
		{
		}
		virtual ~window_base() {
		}

		inline operator HWND() {
			return wnd_ptr_.get();
		}
		// 親ウィンドウハンドルを返す
		//inline HWND get_parent() { return hParent_; }

	private:
		window_base(self_type const&);
		self_type const& operator=(self_type const&);

	public:
		// swap
		void swap(self_type &obj) throw() {
			std::swap( wnd_ptr_, obj.wnd_ptr_ );
			std::swap( ex_style_, obj.ex_style_ );
			std::swap( class_name_, obj.class_name_ );
			std::swap( window_name_, obj.window_name_ );
			std::swap( style_, obj.style_ );
			std::swap( x_, obj.x_ );
			std::swap( y_, obj.y_ );
			std::swap( w_, obj.w_ );
			std::swap( h_, obj.h_ );
			std::swap( hParent_, obj.hParent_ );
			std::swap( hMenu_, obj.hMenu_ );
			std::swap( hInst_, obj.hInst_ );
			std::swap( proc_, obj.proc_ );
			std::swap( child_handler_, obj.child_handler_ );
		}

	public:
		// ウィンドウ操作編
		//   true: 表示
		//  false: 非表示
		inline bool show() {
			return ::ShowWindow( wnd_ptr_.get(), SW_SHOW ) != 0;
		}
		inline bool resize(int x, int y, int w, int h, BOOL repaint = TRUE) {
			return ::MoveWindow(wnd_ptr_.get(), x, y, w, h, repaint) != 0;
		}

	public:
		// リソース類の作成編

		// dtor以外でHWNDが破棄されたときに呼ばれる
		// HWNDハンドラを無効にする
		inline void disable() {
			// デリータを呼ばずにポインタを解放
			wnd_ptr_.release();
		}

		bool create() {
			// ウィンドウを作成
			HWND hWnd = policy::create_window(
				ex_style_,
				const_cast<char_type*>( class_name_.c_str() ),
				const_cast<char_type*>( window_name_.c_str() ),
				style_,
				x_, y_, w_, h_,
				hParent_,
				hMenu_,
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

			// on_create
			on_create<Derived>();

			return true;
		}

	private:
		// ウィンドウ作成後の処理を呼び出す
#ifdef __GNUC__
		template< typename T, typename std::enable_if< lulib::type_traits::has_on_create<T>::value >::type *& = lulib::enabler >
		inline void on_create() {
			reinterpret_cast<Derived*>(this)->on_create();
		}
		template< typename T, typename std::enable_if< !lulib::type_traits::has_on_create<T>::value >::type *& = lulib::enabler >
		inline void on_create() {}
#else
		template< typename T >
		inline void on_create(typename std::enable_if< lulib::type_traits::has_on_create<T>::value >::type * = 0) {
			reinterpret_cast<Derived*>(this)->on_create();
		}
		template< typename T >
		inline void on_create(typename std::enable_if< !lulib::type_traits::has_on_create<T>::value >::type * = 0) {}
#endif

	public:
		// friend関数
		// HINSTANCE の操作
		typedef attribute::instance_handle inst;
		typedef attribute::instance_handle instance;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::instance_handle &&);
		// 親ウィンドウハンドルの操作
		typedef attribute::basic_parent_window_handle<Derived,Char> parent;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::basic_parent_window_handle<D,C> &&);
		// メニューハンドルの操作
		// メニューハンドルを指定する場合
		typedef attribute::basic_menu_handle< ::CreateMenu, Char > menu;
		template<typename D, HMENU (WINAPI *A)(), typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::basic_menu_handle<A,C> &&);
		// 子ウィンドウID(自分自身を示す一意なID。ユーザ指定(enumとか))を指定する場合
		typedef attribute::child_window_id wnd_id;
		typedef attribute::child_window_id window_id;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::child_window_id&&);
		// Window Ex Style の操作
		typedef attribute::ex_style ex_style;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::ex_style &&);
		// Window Style の操作
		typedef attribute::style style;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::style &&);
		// タイトルバーテキスト の操作
		typedef attribute::basic_title<Char> title;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::basic_title<C> &&);
		// ウィンドウ位置の操作
		typedef attribute::position position;
		typedef attribute::position pos;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::position &&);
		// ウィンドウサイズの操作
		typedef attribute::size size;
		template<typename D, typename C>
		friend window_base<D,C>& attribute::operator<<(window_base<D,C>&, attribute::size &&);

	protected:
		wnd_ptr wnd_ptr_;          // HWNDハンドラ
		std::size_t ex_style_;     // ウィンドウ拡張スタイル
		string_type class_name_;   // クラスネーム
		string_type window_name_;  // タイトルバーに表示されるテキスト
		std::size_t style_;        // ウィンドウスタイル
		int x_, y_, w_, h_;        // ウィンドウの座標とサイズ
		HWND hParent_;             // 親ウィンドウハンドル
		HMENU hMenu_;              // HMENU : nullを許容
		HINSTANCE hInst_;          // HINSTANCE
		procedure_type proc_;      // ウィンドウプロシージャ
		child_handler child_handler_;            // 子ウィンドウであるかどうか
	};
}}}//namespace lulib::win32api::window_detail
