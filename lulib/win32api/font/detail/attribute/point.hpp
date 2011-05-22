#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/font/basic_font_fwd.hpp>

namespace lulib { namespace win32api { namespace font_detail { namespace attribute {
	// Font Point Size
	struct point {

		// 演算モード
		struct modes {
			enum enum_t {
				subst,  // 代入
				add,    // 加算
				sub,    // 減算
			};
		};

	public:
		point(int pt) : point_(pt), mode_(modes::subst) {}

		inline int operator()() { return point_; }
		inline modes::enum_t mode() { return mode_; }

		friend point&& operator+(point &&attr);
		friend point&& operator-(point &&attr);

		template<typename Char>
		friend basic_font<Char>& operator<<(basic_font<Char>&, point&&);

	private:
		int point_;
		modes::enum_t mode_;
	};

	// 加算型
	point&& operator+(point &&attr) {
		attr.mode_ = point::modes::add;
		return std::move(attr);
	}
	// 減算型
	point&& operator-(point &&attr) {
		attr.mode_ = point::modes::sub;
		return std::move(attr);
	}

}}}}// namespace lulib::win32api::font_detail::attribute
