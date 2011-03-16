#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>

namespace lulib { namespace win32api { namespace window { namespace attribute {

	// 属性forward
	struct style;
	struct ex_style;
	template<typename T> struct basic_title;
	struct position;
	struct size;

	// friend関数forward
	template<typename T> basic_window<T>& operator<<(basic_window<T>&, ex_style &&);
	template<typename T> basic_window<T>& operator<<(basic_window<T>&, style &&);
	template<typename T> basic_window<T>& operator<<(basic_window<T>&, basic_title<T> &&);
	template<typename T> basic_window<T>& operator<<(basic_window<T>&, position &&);
	template<typename T> basic_window<T>& operator<<(basic_window<T>&, size &&);

}}}}// namespace lulib::win32api::window::attribute
