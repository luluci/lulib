#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	struct style {

	public:
		// 演算モード
		struct modes {
			enum enum_t {
				subst,  // 代入
				add,    // 加算
				sub,    // 減算
			};
		};

	public:
		struct styles {
			enum enum_t {
				
			};
		};

		style(std::size_t s) : style_(s), mode_(modes::subst) {}

		inline std::size_t operator()() { return style_; }
		inline modes::enum_t mode() { return mode_; }

		friend style&& operator+(style &&attr);
		friend style&& operator-(style &&attr);

		template<typename Char>
		friend basic_window<Char>& operator<<(basic_window<Char>&, style&&);

	private:
		std::size_t style_;
		modes::enum_t mode_;
	};

	// 加算型
	style&& operator+(style &&attr) {
		attr.mode_ = style::modes::add;
		return std::move(attr);
	}
	// 減算型
	style&& operator-(style &&attr) {
		attr.mode_ = style::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::window_detail::attribute
