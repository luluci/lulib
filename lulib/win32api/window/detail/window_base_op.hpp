#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/detail/window_base_fwd.hpp>
#include <lulib/win32api/window/detail/attributes.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// windowへhInstanceを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, instance_handle &&hInst) {
		// hInstanceを更新
		wnd.hInst_ = hInst;
		// すでにウィンドウが作成されているなら、hInstanceの更新を適用
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWLP_HINSTANCE, reinterpret_cast<LONG_PTR>(wnd.hInst_) );
		}
		// 終了
		return wnd;
	}

	// windowへhParentを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, basic_parent_window_handle<Derived,Char> &&hParent) {
		// hParentを更新
		wnd.hParent_ = hParent;
		// hParentが
		//   NULL以外: wndは子ウィンドウになる
		//   NULL    : wndは親ウィンドウを持たない
		if (wnd.hParent_ != NULL) wnd.child_handler_.reset( wnd.wnd_ptr_.get() );
		// すでにウィンドウが作成されているなら、hParentの更新を適用
		if (wnd.wnd_ptr_) {
			::SetParent(wnd.wnd_ptr_.get(), wnd.hParent_);
		}
		// 終了
		return wnd;
	}

	// windowへMenuを適用
	// メニューハンドルの場合
	template<typename Derived, HMENU (WINAPI *C)(), typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, basic_menu_handle<C,Char> &&menu) {
		// menuがセットされていたら
		auto menu_opt = menu();  // optional<menu>
		if (menu_opt) {
			// menuを更新
			wnd.hMenu_ = *menu_opt;
		}
		// nullなら
		else {
			wnd.hMenu_ = 0;
		}
		// すでにウィンドウが作成されているなら、hParentの更新を適用
		if (wnd.wnd_ptr_) {
			::SetMenu(wnd.wnd_ptr_.get(), wnd.hMenu_);
		}
		// 終了
		return wnd;
	}

	// 子ウィンドウIDを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, child_window_id &&wnd_id) {
		// IDを指定
		wnd.hMenu_ = reinterpret_cast<HMENU>(wnd_id());
		// すでにウィンドウが作成されているなら、IDの更新を適用
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_long_ptr(
				wnd.wnd_ptr_.get(), GWLP_ID, reinterpret_cast<LONG_PTR>(wnd.hMenu_)
			);
		}
		// 終了
		return wnd;
	}

	// windowへtitleを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, basic_title<Char> &&t) {
		// titleを更新
		wnd.window_name_ = t();
		// すでにウィンドウが作成されているなら、titleの更新を適用
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_text( wnd.wnd_ptr_.get(), wnd.window_name_.c_str() );
		}
		// 終了
		return wnd;
	}

	// windowへExStyleを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, ex_style &&s) {
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

	// windowへstyleを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, style &&s) {
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

	// windowへpositionを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, position &&p) {
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

	// windowへsizeを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, size &&s) {
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

	// windowへrepaintを適用
	template<typename Derived, typename Char>
	window_base<Derived,Char>& operator<<(window_base<Derived,Char>& wnd, repaint_t &&) {
		// すでにウィンドウが作成されているなら、画面の更新
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_EXSTYLE, wnd.ex_style_ );
		}
		// 終了
		return wnd;
	}

}}}}// namespace lulib::win32api::window_detail::attribute
