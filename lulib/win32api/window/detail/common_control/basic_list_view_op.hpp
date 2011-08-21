#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_list_view_fwd.hpp>
#include <lulib/win32api/window/detail/common_control/list_view_policy.hpp>
#include <lulib/win32api/window/detail/common_control/basic_list_view_column.hpp>
#include <lulib/win32api/window/detail/common_control/basic_list_view_item.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// column
	template<typename Char>
	typename basic_list_view<Char>::window_type&
	operator<<(basic_list_view<Char>& lv, basic_list_view_column<Char> const& col) {
		typedef list_view_policy<Char> policy;
		int result = policy::insert_column(lv, col.index(), col);
		if (result == -1) {
			throw ra_error("failed to ListView_InsertColumn()");
		}
		// 終了
		typedef typename basic_list_view<Char>::window_type window_type;
		return dynamic_cast<window_type&>(lv);
	}

	// item
	template<typename Char>
	typename basic_list_view<Char>::window_type&
	operator<<(basic_list_view<Char>& lv, basic_list_view_item<Char> const& item) {
		typedef list_view_policy<Char> policy;
		int result;
		// 1列目ならinsert
		if (item.col() == 0) {
			result = policy::insert_item(lv, item);
		}
		// 1列目以降ならset
		else {
			result = policy::set_item(lv, item);
		}
		if (result == -1) {
			throw ra_error("failed to ListView_SetItem()");
		}
		// 終了
		typedef typename basic_list_view<Char>::window_type window_type;
		return dynamic_cast<window_type&>(lv);
	}

}}}}// namespace lulib::win32api::window_detail::common_control
