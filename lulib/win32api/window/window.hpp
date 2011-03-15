#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_class.hpp>
#include <lulib/win32api/window/basic_window.hpp>
#include <lulib/win32api/window/attributes.hpp>
#include <lulib/win32api/window/procedure.hpp>

namespace lulib { namespace win32api { namespace window {

	// Unicodeフラグ(TCHAR)に依存したデフォルトウィンドウクラス型
	typedef basic_window_class<>        window_class;
	// マルチバイト文字ウィンドウクラス型
	typedef basic_window_class<char>    window_class_a;
	// ワイド文字ウィンドウクラス型
	typedef basic_window_class<wchar_t> window_class_w;

	// Unicodeフラグ(TCHAR)に依存したデフォルトウィンドウ型
	typedef basic_window<>        window;
	// マルチバイト文字ウィンドウ型
	typedef basic_window<char>    window_a;
	// ワイド文字ウィンドウ型
	typedef basic_window<wchar_t> window_w;

}}}// namespace lulib::win32api::window
