#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/detail/policy.hpp>
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
		typedef policy<Char> wnd_policy;

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
		subclass() : proc_() {}
		subclass(proc_type const& proc) : proc_(proc) {}
		subclass(proc_type &&proc) : proc_( std::move(proc) ) {}
		subclass(LRESULT(*proc)(HWND,UINT,WPARAM,LPARAM)) : proc_(proc) {}

		//inline operator proc_type const&() { return proc_; }
		inline proc_type& operator()() { return proc_; }

	private:
		proc_type proc_;
	};

	// サブクラス化
	template<typename CommCtrl, typename Char>
	void subclassing(CommCtrl &ctrl) {
		typedef policy<Char> wnd_policy;
		typedef typename CommCtrl::policy policy;

		// この処理を行う前に、
		//     GWLP_USERDATA へ this をセット
		//     proc_ へのプロシージャの登録
		// が行われていないとセグるはず
		auto proc = procedure<CommCtrl,Char>;
		policy::default_procedure = reinterpret_cast<WNDPROC>(wnd_policy::set_window_long_ptr(
			ctrl.wnd_ptr_.get(), GWL_WNDPROC,
			reinterpret_cast<LONG_PTR>( proc )
		));

		// WNDPROCハンドラのセット
		//ctrl.proc_handler_.reset(policy::default_procedure);
		//ctrl.proc_handler_.get_deleter() = WNDPROC_handler<Char>( ctrl.wnd_ptr_.get() );
		ctrl.proc_handler_(ctrl.wnd_ptr_.get(), policy::default_procedure);
	}

}}}}// namespace lulib::win32api::window::common_control
