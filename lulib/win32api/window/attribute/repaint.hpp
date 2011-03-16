#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/policy.hpp>

namespace lulib { namespace win32api { namespace window { namespace attribute {

	const struct repaint_t {} repaint;

	// operator
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, repaint_t && s) {
		// すでにウィンドウが作成されているなら、画面の更新
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_EXSTYLE, wnd.ex_style_ );
		}
		// 終了
		return wnd;
	}

}}}}// namespace lulib::win32api::window::attribute
