#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/font/basic_font_fwd.hpp>

namespace lulib { namespace win32api { namespace font_detail { namespace attribute {
	// Font Height
	struct height {

		// 演算モード
		struct modes {
			enum enum_t {
				subst,  // 代入
				add,    // 加算
				sub,    // 減算
			};
		};

	public:
		height(int h) : height_(h), mode_(modes::subst) {}

		inline int operator()() { return height_; }
		inline modes::enum_t mode() { return mode_; }

		friend height&& operator+(height &&attr);
		friend height&& operator-(height &&attr);

		template<typename Char>
		friend basic_font<Char>& operator<<(basic_font<Char>&, height&&);

	private:
		int height_;
		modes::enum_t mode_;
	};

	// 加算型
	height&& operator+(height &&attr) {
		attr.mode_ = height::modes::add;
		return std::move(attr);
	}
	// 減算型
	height&& operator-(height &&attr) {
		attr.mode_ = height::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::font_detail::attribute
