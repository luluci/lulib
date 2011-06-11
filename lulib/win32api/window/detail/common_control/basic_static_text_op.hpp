#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_static_text_fwd.hpp>
#include <lulib/win32api/window/detail/common_control/subclass.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// subclassを適用
	template<typename Char>
	basic_static_text<Char>& operator<<(basic_static_text<Char>& ctrl, subclass &&s) {
		typedef basic_static_text<Char> ctrl_type;

		ctrl.proc_ = std::move( s() );
		// すでにウィンドウが作成されていて、プロシージャが有効るなら、subclass化
		if (ctrl.wnd_ptr_ && ctrl.proc_) {
			subclassing<ctrl_type,Char>(ctrl);
		}
		// 終了
		return ctrl;
	}

}}}}// namespace lulib::win32api::window_detail::common_control
