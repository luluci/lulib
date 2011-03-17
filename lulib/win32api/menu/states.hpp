#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/state/enable.hpp>
#include <lulib/win32api/menu/state/disabled.hpp>
#include <lulib/win32api/menu/state/checked.hpp>
#include <lulib/win32api/menu/state/hilite.hpp>
#include <lulib/win32api/menu/state/defitem.hpp>
#include <lulib/win32api/menu/state/multi.hpp>

namespace lulib { namespace win32api { namespace menu {

	// 状態
	// 通常
	typedef state::enable    enable;
	typedef state::enable    normal;
	// 無効
	typedef state::disabled  disabled;
	// チェックマーク
	typedef state::checked   checked;
	// ハイライト
	typedef state::hilite    hilite;
	// デフォルト選択アイテム
	typedef state::defitem   defitem;
	// 複合状態
	typedef detail::state    states;
	typedef state::multi     multi;

}}}// namespace lulib::win32api::window
