#pragma once
#pragma warning(disable : 4819)

#include <lulib/singleton/singleton.hpp>
#include <lulib/win32api/window/detail/common_control/init_common_control_ex.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// ロード: リストビュー、ヘッダーコントロール
		// ロード: ツリービュー、ツールチップ
		// ロード: ツールバー、ステータスバー、トラックバー、ツールチップ
		// ロード: タブコントロール、ツールチップ
		// ロード: アップダウンコントロール
		// ロード: プログレスバー
		// ロード: ホットキーコントロール
		// ロード: アニメートコントロール
		// ロード: 以上のすべてのコントロール
		typedef init_common_control_ex<ICC_WIN95_CLASSES> init_win95_classes_impl;
	}

	typedef lulib::singleton<detail::init_win95_classes_impl> init_win95_classes;

}}}}// namespace lulib::win32api::window_detail::common_control
