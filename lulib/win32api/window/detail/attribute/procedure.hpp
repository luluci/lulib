#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <functional>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// プロシージャファンクタ
	class procedure {
		// ウィンドウプロシージャコールバックファンクタ
		typedef std::function<LRESULT(HWND,UINT,WPARAM,LPARAM)> proc_type;

	public:
		procedure(proc_type const& proc) : proc_(proc) {}
		procedure(proc_type &&proc) : proc_(proc) {}
		procedure(LRESULT(*proc)(HWND,UINT,WPARAM,LPARAM)) : proc_(proc) {}

		//inline operator proc_type const&() { return proc_; }
		inline proc_type& operator()() { return proc_; }

	private:
		proc_type proc_;
	};

}}}}// namespace lulib::win32api::window::attribute
