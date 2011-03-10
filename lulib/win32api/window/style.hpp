#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>

namespace lulib { namespace win32api { namespace window {

	struct style {

	public:
		struct styles {
			enum enum_t {
				
			};
		};

		style(std::size_t s) : style_(s), mode_(subst) {}

		friend style&& operator+(style &&s);
		friend style&& operator-(style &&s);
		template<typename Char>
		friend basic_window<Char>& operator<<(basic_window<Char>&, style&&);

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
	style&& operator+(style &&s) {
		s.mode_ = style::add;
		return std::move(s);
	}
	// 減算型
	style&& operator-(style &&s) {
		s.mode_ = style::sub;
		return std::move(s);
	}
	// operator
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, style &&s) {
		// styleを更新
		switch (s.mode_) {
			case style::subst: {
				wnd.style_ = s.style_;
				break;
			}
			case style::add: {
				wnd.style_ |= s.style_;
				break;
			}
			case style::sub: {
				wnd.style_ &= ~s.style_;
				break;
			}
		}
		// すでにウィンドウが作成されているなら、ex_styleの更新を適用
		if (wnd.wnd_ptr_) {
			window_traits<Char>::set_window_long_ptr( wnd.wnd_ptr_.get(), GWL_STYLE, wnd.style_ );
		}
		// 終了
		return wnd;
	}

}}}// namespace lulib::win32api::window
