#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/detail/attributes.hpp>
//#include <lulib/win32api/window/detail/window_policy.hpp>

namespace lulib { namespace win32api { namespace window_detail {

	namespace attribute {

		// windowへWindowClassNameを適用
		template<typename Char>
		typename basic_window<Char>::window_type&
		operator<<(basic_window<Char> &wnd, basic_window_class_name<Char> &&wnd_cls) {
			// すでにウィンドウが作成されているなら、WindowClassの変更はできないっぽ
			if (wnd.wnd_ptr_) {
				return wnd;
			}
			// ClassNameを更新
			wnd.class_name_ = wnd_cls;
			// 終了
			typedef typename basic_window<Char>::window_type window_type;
			return dynamic_cast<window_type&>(wnd);
		}

		// windowへprocedureを適用
		template<typename Char>
		typename basic_window<Char>::window_type&
		operator<<(basic_window<Char> &wnd, attribute::procedure &&proc) {
			// procを更新
			wnd.proc_ = std::move( proc() );
			// 終了
			typedef typename basic_window<Char>::window_type window_type;
			return dynamic_cast<window_type&>(wnd);
		}

	}// namespace lulib::win32api::window_detail::attribute

}}}// namespace lulib::win32api::window_detail
