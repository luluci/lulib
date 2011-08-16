#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <memory>
#include <functional>
#include <type_traits>


#include <lulib/win32api/window/detail/window_base_op.hpp>
#include <lulib/win32api/window/detail/window_policy.hpp>
#include <lulib/win32api/window/detail/procedure.hpp>

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

	template<typename Derived, typename Window, typename Char>
	class window_base {
		// self_type
		typedef window_base<Derived, Window, Char> self_type;

	public:
		// 最上位派生クラス
		// 大元のクラス
		// どのタイプのウィンドウか
		typedef Window window_type;

	private:
		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

		// WindowBaseポリシー
		typedef ::lulib::win32api::window_detail::window_policy<Char> wnd_policy;

		// HWNDハンドラ
		// HWNDが生きたままwindow_baseが破棄されるとき、デリータが呼ばれる
		// これは共有しないよ！
		typedef std::unique_ptr<HWND, HWND_deleter> wnd_ptr;

		// 自分が子ウィンドウの場合、破棄されるときには親ウィンドウとの関連を外す
		typedef std::unique_ptr<HWND, HWND_handler> child_handler;

		// ウィンドウプロシージャコールバック
		typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> procedure_type;

		// WindowProcedure
		typedef LRESULT (CALLBACK* wnd_proc)(HWND,UINT,WPARAM,LPARAM);

	protected:
		// 継承して使う
		window_base()
			: wnd_ptr_(), ex_style_(0), class_name_(), window_name_()
			, style_(WS_OVERLAPPEDWINDOW), x_(0), y_(0), w_(0), h_(0), hParent_(NULL)
			, hMenu_(NULL), hInst_(NULL), proc_(), child_handler_()
		{
		}
		virtual ~window_base() {
		}

	public:
		inline operator HWND() {
			return wnd_ptr_.get();
		}

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
			HWND hWnd = wnd_policy::create_window(
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
			wnd_policy::set_window_long_ptr(
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
		// プロシージャの呼び出し
		// ウィンドウの場合:
		//     ユーザが必ず手動で設定する
		//     設定されていない場合、デフォルトウィンドウプロシージャに任せる
		//     basic_windowのコンストラクタで default_proc_ はセットされるので、
		//     基本的にnullで使われることはない
		// コモンコントロールの場合:
		//     サブクラス化がされていない限り、この関数が呼ばれることはない
		//     (サブクラス化がされて初めてprocedureが呼び出される)
		//     サブクラス化と同時に default_proc_ もセットされるので、
		//     基本的にnullで使われることはない
		inline LRESULT callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			// プロシージャがセットされているなら実行
			if ( this->proc_ ) {
				return this->proc_(hWnd, msg, wParam, lParam);
			}
			// プロシージャがセットされていないなら、DefWindowProcにまかせる
			else {
				// 一応チェック
				if (default_proc_) return default_proc_(hWnd, msg, wParam, lParam);
				else return 0;
			}
		}

		// デフォルトウィンドウプロシージャ
		static LRESULT default_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			if (default_proc_) {
				return default_proc_(hWnd, msg, wParam, lParam);
			}
			else {
				return 0;
			}
		}

	public:
		// friend関数
		// HINSTANCE の操作
		typedef attribute::instance_handle inst;
		typedef attribute::instance_handle instance;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::instance_handle &&);
		// 親ウィンドウハンドルの操作
		typedef attribute::basic_parent_window_handle<Derived,Window,Char> parent;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::basic_parent_window_handle<D,W,C> &&);
		// メニューハンドルの操作
		// メニューハンドルを指定する場合
		typedef attribute::basic_menu_handle< ::CreateMenu, Char > menu;
		template<typename D, typename W, HMENU (WINAPI *A)(), typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::basic_menu_handle<A,C> &&);
		// 子ウィンドウID(自分自身を示す一意なID。ユーザ指定(enumとか))を指定する場合
		typedef attribute::child_window_id wnd_id;
		typedef attribute::child_window_id window_id;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::child_window_id&&);
		// Window Ex Style の操作
		typedef attribute::ex_style ex_style;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::ex_style &&);
		// Window Style の操作
		typedef attribute::style style;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::style &&);
		// タイトルバーテキスト の操作
		typedef attribute::basic_title<Char> title;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::basic_title<C> &&);
		// ウィンドウ位置の操作
		typedef attribute::position position;
		typedef attribute::position pos;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::position &&);
		// ウィンドウサイズの操作
		typedef attribute::size size;
		template<typename D, typename W, typename C>
		friend typename window_base<D,W,C>::window_type&
		attribute::operator<<(window_base<D,W,C>&, attribute::size &&);

	protected:
		wnd_ptr wnd_ptr_;              // HWNDハンドラ
		std::size_t ex_style_;         // ウィンドウ拡張スタイル
		string_type class_name_;       // クラスネーム
		string_type window_name_;      // タイトルバーに表示されるテキスト
		std::size_t style_;            // ウィンドウスタイル
		int x_, y_, w_, h_;            // ウィンドウの座標とサイズ
		HWND hParent_;                 // 親ウィンドウハンドル
		HMENU hMenu_;                  // HMENU : nullを許容
		HINSTANCE hInst_;              // HINSTANCE
		procedure_type proc_;          // ウィンドウプロシージャ
		child_handler child_handler_;  // 子ウィンドウであるかどうか

		static wnd_proc default_proc_;  // デフォルトウィンドウプロシージャ
	};
	template<typename D, typename W, typename C>
	typename window_base<D,W,C>::wnd_proc window_base<D,W,C>::default_proc_ = 0;

}}}//namespace lulib::win32api::window_detail
