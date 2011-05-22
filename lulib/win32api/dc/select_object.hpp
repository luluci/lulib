#pragma once

#include <windows.h>

#include <lulib/win32api/exceptions.hpp>

#include <sstream>

namespace lulib { namespace win32api { namespace dc_detail {

	// SelectObjectラッパー
	// 自動でSelectし直す
	template<typename Object>
	struct select_object {
		typedef select_object<Object> self_type;

		select_object() : hdc_(NULL), org_obj_(NULL) {}
		select_object(HDC hdc) : hdc_(hdc), org_obj_(NULL) {}
		select_object(HDC hdc, Object obj) : hdc_(hdc), org_obj_(obj) {
			org_obj_ = reinterpret_cast<Object>( ::SelectObject(hdc_, obj) );
			if (org_obj_ == NULL) throw ra_error("failed: select object.");
		}
		~select_object() {
			if (org_obj_ != NULL) {
				if ( ::SelectObject(hdc_, org_obj_) == NULL ) {
					//throw ra_error("failed: select object.");
				}
			}
		}

		self_type& operator=(HDC hDC) {
			hdc_ = hDC;
			return *this;
		}
		self_type& operator=(Object obj) {
			if (org_obj_ == 0) {
				org_obj_ = reinterpret_cast<Object>( ::SelectObject(hdc_, obj) );
				if (org_obj_ == NULL) throw ra_error("failed: select object.");
			} else {
				if ( ::SelectObject(hdc_, obj) == NULL ) throw ra_error("failed: select object.");
			}
			return *this;
		}

	private:
		HDC hdc_;
		Object org_obj_;
	};

}}}// namespace lulib::win32api::dc_detail
