#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/detail/common_control/common_control_base_fwd.hpp>
#include <lulib/win32api/window/detail/common_control/ex_style.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// CommonControlへExStyleを適用
	template<typename Derived, typename Window, typename Char>
	typename common_control_base<Derived,Window,Char>::window_type&
	operator<<(common_control_base<Derived,Window,Char>& ctrl, ex_style && s) {
		typedef typename common_control_base<Derived,Window,Char>::window_type cc_type;  // CommonControl Type
		typedef typename cc_type::policy policy;  // CommonControl Policy Type

		// ex_styleを更新
		switch (s.mode()) {
			case ex_style::modes::subst: {
				ctrl.cc_ex_style_ = s();
				break;
			}
			case ex_style::modes::add: {
				ctrl.cc_ex_style_ |= s();
				break;
			}
			case ex_style::modes::sub: {
				ctrl.cc_ex_style_ &= ~s();
				break;
			}
		}
		// すでにウィンドウが作成されているなら、ex_styleの更新を適用
		if (ctrl.wnd_ptr_) {
			policy::set_ex_style(ctrl, ctrl.cc_ex_style_);
		}
		// 終了
		return dynamic_cast<cc_type&>(ctrl);
	}

}}}}// namespace lulib::win32api::window_detail::common_control
