#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/policy.hpp>

namespace lulib { namespace win32api { namespace window { namespace attribute {

	struct size {

	public:
		size(int w, int h, bool repaint = true)
			: w_(w), h_(h), repaint_(repaint), mode_(subst)
		{}
		size(RECT const& rc, bool repaint = true)
			: w_(rc.right - rc.left), h_(rc.bottom - rc.top), repaint_(repaint), mode_(subst)
		{}

		friend size&& operator+(size &&s);
		friend size&& operator-(size &&s);
		template<typename Char>
		friend basic_window<Char>& operator<<(basic_window<Char>&, size&&);

	private:
		// width, height
		// -1を指定した変数は無視する
		int w_, h_;
		// 更新時に再描画するか否か
		bool repaint_;

		enum enum_t {
			subst,  // 代入
			add,    // 加算
			sub,    // 減算
		};
		enum_t mode_;
	};

	// 加算型
	size&& operator+(size &&s) {
		s.mode_ = size::add;
		return std::move(s);
	}
	// 減算型
	size&& operator-(size &&s) {
		s.mode_ = size::sub;
		return std::move(s);
	}
	// operator
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, size &&s) {
		// styleを更新
		switch (s.mode_) {
			case position::subst: {
				if (s.w_ != -1) wnd.w_ = s.w_;
				if (s.h_ != -1) wnd.h_ = s.h_;
				break;
			}
			case position::add: {
				if (s.w_ != -1) wnd.w_ += s.w_;
				if (s.h_ != -1) wnd.h_ += s.h_;
				break;
			}
			case position::sub: {
				if (s.w_ != -1) wnd.w_ -= s.w_;
				if (s.h_ != -1) wnd.h_ -= s.h_;
				break;
			}
		}
		// すでにウィンドウが作成されているなら、positionの更新を適用
		if (wnd.wnd_ptr_) {
			// 再描画は別途実行
			::MoveWindow( wnd.wnd_ptr_.get(), wnd.x_, wnd.y_, wnd.w_, wnd.h_, s.repaint_ );
		}
		// 終了
		return wnd;
	}

}}}}// namespace lulib::win32api::window::attribute
