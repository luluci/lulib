#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/detail/attributes.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

namespace lulib { namespace win32api { namespace window_detail {

	namespace attribute {

		// operator<<
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, basic_title<Char> &&t) {
			// titleを更新
			wnd.window_name_ = t();
			// すでにウィンドウが作成されているなら、titleの更新を適用
			if (wnd.wnd_ptr_) {
				policy<Char>::set_window_text( wnd.wnd_ptr_.get(), wnd.window_name_.c_str() );
			}
			// 終了
			return wnd;
		}

		// operator
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, ex_style && s) {
			// ex_styleを更新
			switch (s.mode()) {
				case ex_style::modes::subst: {
					wnd.ex_style_ = s();
					break;
				}
				case ex_style::modes::add: {
					wnd.ex_style_ |= s();
					break;
				}
				case ex_style::modes::sub: {
					wnd.ex_style_ &= ~s();
					break;
				}
			}
			// すでにウィンドウが作成されているなら、ex_styleの更新を適用
			if (wnd.wnd_ptr_) {
				policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_EXSTYLE, wnd.ex_style_ );
			}
			// 終了
			return wnd;
		}

		// operator
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, style &&s) {
			// styleを更新
			switch (s.mode()) {
				case style::modes::subst: {
					wnd.style_ = s();
					break;
				}
				case style::modes::add: {
					wnd.style_ |= s();
					break;
				}
				case style::modes::sub: {
					wnd.style_ &= ~s();
					break;
				}
			}
			// すでにウィンドウが作成されているなら、ex_styleの更新を適用
			if (wnd.wnd_ptr_) {
				policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_STYLE, wnd.style_ );
			}
			// 終了
			return wnd;
		}

		// operator
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, position &&p) {
			// styleを更新
			switch (p.mode()) {
				case position::modes::subst: {
					if (p.x() != -1) wnd.x_ = p.x();
					if (p.y() != -1) wnd.y_ = p.y();
					if (p.w() != -1) wnd.w_ = p.w();
					if (p.h() != -1) wnd.h_ = p.h();
					break;
				}
				case position::modes::add: {
					if (p.x() != -1) wnd.x_ += p.x();
					if (p.y() != -1) wnd.y_ += p.y();
					if (p.w() != -1) wnd.w_ += p.w();
					if (p.h() != -1) wnd.h_ += p.h();
					break;
				}
				case position::modes::sub: {
					if (p.x() != -1) wnd.x_ -= p.x();
					if (p.y() != -1) wnd.y_ -= p.y();
					if (p.w() != -1) wnd.w_ -= p.w();
					if (p.h() != -1) wnd.h_ -= p.h();
					break;
				}
			}
			// すでにウィンドウが作成されているなら、positionの更新を適用
			if (wnd.wnd_ptr_) {
				// 再描画は別途実行
				::MoveWindow( wnd.wnd_ptr_.get(), wnd.x_, wnd.y_, wnd.w_, wnd.h_, p.repaint() );
			}
			// 終了
			return wnd;
		}

		// operator
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, size &&s) {
			// styleを更新
			switch (s.mode()) {
				case size::modes::subst: {
					if (s.w() != -1) wnd.w_ = s.w();
					if (s.h() != -1) wnd.h_ = s.h();
					break;
				}
				case size::modes::add: {
					if (s.w() != -1) wnd.w_ += s.w();
					if (s.h() != -1) wnd.h_ += s.h();
					break;
				}
				case size::modes::sub: {
					if (s.w() != -1) wnd.w_ -= s.w();
					if (s.h() != -1) wnd.h_ -= s.h();
					break;
				}
			}
			// すでにウィンドウが作成されているなら、positionの更新を適用
			if (wnd.wnd_ptr_) {
				// 再描画は別途実行
				::MoveWindow( wnd.wnd_ptr_.get(), wnd.x_, wnd.y_, wnd.w_, wnd.h_, s.repaint() );
			}
			// 終了
			return wnd;
		}

		// operator
		template<typename Char>
		basic_window<Char>& operator<<(basic_window<Char>& wnd, repaint_t &&) {
			// すでにウィンドウが作成されているなら、画面の更新
			if (wnd.wnd_ptr_) {
				policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_EXSTYLE, wnd.ex_style_ );
			}
			// 終了
			return wnd;
		}

	}// namespace lulib::win32api::window_detail::attribute

}}}// namespace lulib::win32api::window_detail
