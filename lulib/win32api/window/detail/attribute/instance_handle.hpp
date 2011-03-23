#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

	// HINSTANCE型
	class instance_handle {

	public:
		instance_handle(HINSTANCE hInst) : hInst_(hInst) {}

		inline operator HINSTANCE() { return hInst_; }

	private:
		HINSTANCE hInst_;
	};

}}}}// namespace lulib::win32api::window::attribute
