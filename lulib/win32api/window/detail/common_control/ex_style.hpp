#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// コモンコントロール固有拡張スタイル
	class ex_style {

	public:
		// 演算モード
		struct modes {
			enum enum_t {
				subst,  // 代入
				add,    // 加算
				sub,    // 減算
			};
		};

		ex_style(std::size_t style) : style_(style), mode_(modes::subst) {}

		inline std::size_t operator()() { return style_; }
		inline modes::enum_t mode() { return mode_; }

		friend ex_style&& operator+(ex_style &&attr);
		friend ex_style&& operator-(ex_style &&attr);

	private:
		std::size_t style_;
		modes::enum_t mode_;
	};

	// 加算型
	ex_style&& operator+(ex_style &&attr) {
		attr.mode_ = ex_style::modes::add;
		return std::move(attr);
	}
	// 減算型
	ex_style&& operator-(ex_style &&attr) {
		attr.mode_ = ex_style::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::window::common_control
