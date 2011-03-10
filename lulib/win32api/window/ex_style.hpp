#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>

namespace lulib { namespace win32api { namespace window {

	struct ex_style {

	public:
		struct styles {
			enum enum_t {
				
			};
		};

		ex_style(std::size_t style) : style_(style), mode_(subst) {}

		friend ex_style&& operator+(ex_style &&s);
		friend ex_style&& operator-(ex_style &&s);
		template<typename Char>
		friend basic_window<Char>& operator<<(basic_window<Char>&, ex_style &&);

	private:
		std::size_t style_;

		enum enum_t {
			subst,  // 代入
			add,    // 加算
			sub,    // 減算
		};
		enum_t mode_;
	};

	// 加算型
	ex_style&& operator+(ex_style &&s) {
		s.mode_ = ex_style::add;
		return std::move(s);
	}
	// 減算型
	ex_style&& operator-(ex_style &&s) {
		s.mode_ = ex_style::sub;
		return std::move(s);
	}
	// operator
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, ex_style && s) {
		// ex_styleを更新
		switch (s.mode_) {
			case ex_style::subst: {
				wnd.ex_style_ = s.style_;
				break;
			}
			case ex_style::add: {
				wnd.ex_style_ |= s.style_;
				break;
			}
			case ex_style::sub: {
				wnd.ex_style_ &= ~s.style_;
				break;
			}
		}
		// すでにウィンドウが作成されているなら、ex_styleの更新を適用
		if (wnd.wnd_ptr_) {
			window_traits<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_EXSTYLE, wnd.ex_style_ );
		}
		// 終了
		return wnd;
	}

}}}// namespace lulib::win32api::window
