#pragma once
#pragma warning(disable : 4819)

#include <lulib/singleton/singleton.hpp>
#include <lulib/win32api/window/detail/common_control/init_common_control_ex.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// ロード: ツリービュー、ツールチップ
		typedef init_common_control_ex<ICC_TREEVIEW_CLASSES> init_treeview_classes_impl;
	}

	typedef lulib::singleton<detail::init_treeview_classes_impl> init_treeview_classes;

}}}}// namespace lulib::win32api::window_detail::common_control
