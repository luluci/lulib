#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/detail/attributes.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

namespace lulib { namespace win32api { namespace window_detail {

	namespace attribute {

		// windowへWindowClassNameを適用
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, basic_window_class_name<Char> &&wnd_cls) {
			// すでにウィンドウが作成されているなら、WindowClassの変更はできないっぽ
			if (wnd.wnd_ptr_) {
				return wnd;
			}
			// ClassNameを更新
			wnd.class_name_ = wnd_cls;
			// 終了
			return wnd;
		}

	}// namespace lulib::win32api::window_detail::attribute

}}}// namespace lulib::win32api::window_detail
