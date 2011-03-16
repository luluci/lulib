#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/attribute/style.hpp>
#include <lulib/win32api/window/attribute/ex_style.hpp>
#include <lulib/win32api/window/attribute/title.hpp>
#include <lulib/win32api/window/attribute/position.hpp>
#include <lulib/win32api/window/attribute/size.hpp>

namespace lulib { namespace win32api { namespace window {

	// 属性
	typedef attribute::style    style;
	typedef attribute::ex_style ex_style;
	typedef attribute::basic_title<TCHAR>    title;
	typedef attribute::basic_title<char>     title_a;
	typedef attribute::basic_title<wchar_t>  title_w;
	typedef attribute::position position;
	typedef attribute::position pos;
	typedef attribute::size     size;

}}}// namespace lulib::win32api::window
