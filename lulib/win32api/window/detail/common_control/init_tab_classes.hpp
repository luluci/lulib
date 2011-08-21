#pragma once
#pragma warning(disable : 4819)

#include <lulib/singleton/singleton.hpp>
#include <lulib/win32api/window/detail/common_control/init_common_control_ex.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// ロード: タブコントロール、ツールチップ
		typedef init_common_control_ex<ICC_TAB_CLASSES> init_tab_classes_impl;
	}

	typedef lulib::singleton<detail::init_tab_classes_impl> init_tab_classes;

}}}}// namespace lulib::win32api::window_detail::common_control
