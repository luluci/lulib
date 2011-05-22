#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/font/basic_font_fwd.hpp>
#include <lulib/win32api/font/detail/attributes.hpp>
#include <lulib/win32api/font/detail/policy.hpp>

#include <lulib/c_str/print.hpp>

namespace lulib { namespace win32api { namespace font_detail { namespace attribute {

	// LOGPIXELSYを取得
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, init &&attr) {
		HDC hDC = ::GetDC( attr() );
		font.log_pixels_y_ = ::GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(attr(), hDC);
		// 終了
		return font;
	}

	// fontへPointSizeを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, point &&attr) {
		// pointを更新
		switch (attr.mode()) {
			case height::modes::subst: {
				font.pt_ = attr();
				break;
			}
			case height::modes::add: {
				font.pt_ += attr();
				break;
			}
			case height::modes::sub: {
				font.pt_ -= attr();
				break;
			}
		}
		// PointSizeを計算
		if (font.log_pixels_y_) font.log_font_.lfHeight = -((font.pt_ * *font.log_pixels_y_) / 72);
		else font.log_font_.lfHeight = -font.pt_;
		// 終了
		return font;
	}

	// fontへheightを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, height &&attr) {
		// heightを更新
		switch (attr.mode()) {
			case height::modes::subst: {
				font.log_font_.lfHeight = attr();
				break;
			}
			case height::modes::add: {
				font.log_font_.lfHeight += attr();
				break;
			}
			case height::modes::sub: {
				font.log_font_.lfHeight -= attr();
				break;
			}
		}
		// 終了
		return font;
	}

	// fontへregularを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, regular_t const&) {
		// regular
		font.log_font_.lfWeight = FW_REGULAR;
		// 終了
		return font;
	}

	// fontへboldを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, bold_t const&) {
		// regular
		font.log_font_.lfWeight = FW_BOLD;
		// 終了
		return font;
	}

	// fontへunderlineを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, underline_t const&) {
		// regular
		font.log_font_.lfUnderline = 1;
		// 終了
		return font;
	}

	// fontへFontFaceを適用
	template<typename Char>
	basic_font<Char>& operator<<(basic_font<Char>& font, basic_face<Char> &&face) {
		// face
		font.face_ = face();
		// set
		lulib::c_str::print(font.log_font_.lfFaceName, font.face_.c_str());
		// 終了
		return font;
	}

}}}}// namespace lulib::win32api::font_detail::attribute
