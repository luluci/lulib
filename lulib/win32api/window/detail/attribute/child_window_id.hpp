#pragma once
#pragma warning(disable : 4819)

#include <cstdint>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// CreateWindowExの第10引数から指定する子ウィンドウIDの設定
	struct child_window_id {
		child_window_id(std::uint16_t id) : id_(id) {}

		inline std::uint16_t operator()() { return id_; }

	private:
		std::uint16_t id_;
	};

}}}}// namespace lulib::win32api::window_detail::attribute
