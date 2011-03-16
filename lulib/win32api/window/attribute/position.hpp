#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/policy.hpp>

namespace lulib { namespace win32api { namespace window { namespace attribute {

	struct position {

	public:
		position(int x, int y, bool repaint = true)
			: x_(x), y_(y), w_(-1), h_(-1), repaint_(repaint), mode_(subst)
		{}
		position(int x, int y, int w, int h, bool repaint = true)
			: x_(x), y_(y), w_(w), h_(h), repaint_(repaint), mode_(subst)
		{}
		position(RECT const& rc, bool repaint = true)
			: x_(rc.left), y_(rc.top), w_(rc.right - rc.left), h_(rc.bottom - rc.top), repaint_(repaint), mode_(subst)
		{}

		friend position&& operator+(position &&s);
		friend position&& operator-(position &&s);
		template<typename Char>
		friend basic_window<Char>& operator<<(basic_window<Char>&, position&&);

	private:
		// x座標, y座標, width, height
		// -1を指定した変数は無視する
		int x_, y_, w_, h_;
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
	position&& operator+(position &&p) {
		p.mode_ = position::add;
		return std::move(p);
	}
	// 減算型
	position&& operator-(position &&p) {
		p.mode_ = position::sub;
		return std::move(p);
	}
	// operator
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, position &&p) {
		// styleを更新
		switch (p.mode_) {
			case position::subst: {
				if (p.x_ != -1) wnd.x_ = p.x_;
				if (p.y_ != -1) wnd.y_ = p.y_;
				if (p.w_ != -1) wnd.w_ = p.w_;
				if (p.h_ != -1) wnd.h_ = p.h_;
				break;
			}
			case position::add: {
				if (p.x_ != -1) wnd.x_ += p.x_;
				if (p.y_ != -1) wnd.y_ += p.y_;
				if (p.w_ != -1) wnd.w_ += p.w_;
				if (p.h_ != -1) wnd.h_ += p.h_;
				break;
			}
			case position::sub: {
				if (p.x_ != -1) wnd.x_ -= p.x_;
				if (p.y_ != -1) wnd.y_ -= p.y_;
				if (p.w_ != -1) wnd.w_ -= p.w_;
				if (p.h_ != -1) wnd.h_ -= p.h_;
				break;
			}
		}
		// すでにウィンドウが作成されているなら、positionの更新を適用
		if (wnd.wnd_ptr_) {
			// 再描画は別途実行
			::MoveWindow( wnd.wnd_ptr_.get(), wnd.x_, wnd.y_, wnd.w_, wnd.h_, p.repaint_ );
		}
		// 終了
		return wnd;
	}

}}}}// namespace lulib::win32api::window::attribute
