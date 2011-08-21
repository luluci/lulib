#pragma once
#pragma warning(disable : 4819)

#include <lulib/singleton/singleton.hpp>
#include <lulib/win32api/window/detail/common_control/init_common_control_ex.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// ロード: Version 4.71 以降： IP アドレスコントロール
		typedef init_common_control_ex<ICC_INTERNET_CLASSES> init_internet_classes_impl;
	}

	typedef lulib::singleton<detail::init_internet_classes_impl> init_internet_classes;

}}}}// namespace lulib::win32api::window_detail::common_control
