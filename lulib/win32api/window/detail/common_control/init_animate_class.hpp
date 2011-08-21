#pragma once
#pragma warning(disable : 4819)

#include <lulib/singleton/singleton.hpp>
#include <lulib/win32api/window/detail/common_control/init_common_control_ex.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// ロード: アニメートコントロール
		typedef init_common_control_ex<ICC_ANIMATE_CLASS> init_animate_class_impl;
	}

	typedef lulib::singleton<detail::init_animate_class_impl> init_animate_class;

}}}}// namespace lulib::win32api::window_detail::common_control
