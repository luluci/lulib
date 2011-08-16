#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/common_control/common_control_base_fwd.hpp>
#include <lulib/win32api/window/detail/window_policy.hpp>
#include <lulib/win32api/window/detail/procedure.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	/*
	template<typename Char>
	struct WNDPROC_handler {
		typedef WNDPROC pointer;
		void operator()(WNDPROC org_proc) {
			// デフォルトプロシージャに戻す
			typedef policy<Char> policy;
			if (hWnd_) {
				policy::set_window_long_ptr(
					hWnd_, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(org_proc)
				);
			}
		}

		WNDPROC_handler() : hWnd_() {}
		WNDPROC_handler(HWND hWnd) : hWnd_(hWnd) {}
		HWND hWnd_;
	};
	*/
	template<typename Char>
	struct WNDPROC_handler {
		typedef window_policy<Char> wnd_policy;

		WNDPROC_handler() : hWnd_(), proc_() {}
		WNDPROC_handler(HWND hWnd, WNDPROC proc) : hWnd_(hWnd), proc_(proc) {}

		~WNDPROC_handler() {
			if (hWnd_) {
				wnd_policy::set_window_long_ptr(
					hWnd_, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(proc_)
				);
			}
		}

		inline operator bool() { return (hWnd_ != 0); }
		void operator()(HWND hWnd, WNDPROC proc) {
			hWnd_ = hWnd;
			proc_ = proc;
		}
		inline WNDPROC get() { return proc_; }

	private:
		HWND hWnd_;
		WNDPROC proc_;
	};

	// プロシージャ
	class subclass {
		// ウィンドウプロシージャコールバックファンクタ
		typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> proc_type;

	public:
		subclass() : proc_() {}  // subclass化解除用
		subclass(proc_type const& proc) : proc_(proc) {}
		subclass(proc_type &&proc) : proc_( std::move(proc) ) {}
		subclass(LRESULT(*proc)(HWND,UINT,WPARAM,LPARAM)) : proc_(proc) {}

		inline proc_type& operator()() { return proc_; }

	private:
		proc_type proc_;
	};

	// サブクラス化
	template<typename Ctrl, typename Char>
	void subclassing(Ctrl &ctrl) {
		typedef window_policy<Char> wnd_policy;

		// この処理を行う前に、
		//     GWLP_USERDATA へ this をセット
		//     proc_ へのプロシージャの登録
		// が行われていないとセグるはず
		// インターフェースとなるプロシージャを取得
		auto proc = procedure<Ctrl,Char>;
		// サブクラス化を実行
		// オリジナルのプロシージャは default_proc_ に設定
		// ("default_proc_ == 0" の場合のみこの関数は呼ばれる
		//  "default_proc_ != 0" のとき、すでにサブクラス化実施済み)
		Ctrl::default_proc_ = reinterpret_cast<WNDPROC>(wnd_policy::set_window_long_ptr(
			ctrl.wnd_ptr_.get(), GWL_WNDPROC,
			reinterpret_cast<LONG_PTR>( proc )
		));

		// WNDPROCハンドラのセット
		//ctrl.proc_handler_.reset(policy::default_procedure);
		//ctrl.proc_handler_.get_deleter() = WNDPROC_handler<Char>( ctrl.wnd_ptr_.get() );
		ctrl.proc_handler_(ctrl.wnd_ptr_.get(), Ctrl::default_proc_);
	}

	// subclassを適用
	template<typename Derived, typename Window, typename Char>
	typename common_control_base<Derived,Window,Char>::window_type&
	operator<<(common_control_base<Derived,Window,Char>& ctrl, subclass &&s) {
		typedef common_control_base<Derived,Window,Char> ctrl_type;

		// ユーザ指定のプロシージャをセット
		// これにWMを転送する
		ctrl.proc_ = std::move( s() );
		// サブクラス化が未実行(default_proc_ == 0) かつ
		// すでにウィンドウが作成されていて、プロシージャが有効なら、subclass化を実行
		if (ctrl_type::default_proc_ == 0 && ctrl.wnd_ptr_ && ctrl.proc_) {
			subclassing<ctrl_type,Char>(ctrl);
		}
		// 終了
		typedef typename common_control_base<Derived,Window,Char>::window_type window_type;
		return dynamic_cast<window_type&>(ctrl);
	}

}}}}// namespace lulib::win32api::window::common_control
