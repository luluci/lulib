#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace window {

	// char 特性
	template<typename Char>
	struct window_traits;

	template<>
	struct window_traits<char> {
		// WNDCLASSEX
		typedef WNDCLASSEXA wnd_class;

		// ::RegisterClassEx
		inline static ATOM register_class(wnd_class *wc) {
			return ::RegisterClassExA(wc);
		}

		// ::CreateWindowEx
		inline static HWND create_window(
			DWORD dwExStyle,
			LPCSTR lpClassName,
			LPCSTR lpWindowName,
			DWORD dwStyle,
			int x,
			int y,
			int nWidth,
			int nHeight,
			HWND hWndParent,
			HMENU hMenu,
			HINSTANCE hInstance,
			LPVOID lpParam
		) {
			return ::CreateWindowExA(
				dwExStyle,
				lpClassName,
				lpWindowName,
				dwStyle,
				x, y, nWidth, nHeight,
				hWndParent,
				hMenu,
				hInstance,
				lpParam
			);
		}

		// ::DefWindowProc
		inline static LRESULT def_window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
			return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
		}

		// ::GetWindowLongPtr
		inline static LONG_PTR get_window_long_ptr(HWND hWnd, int nIndex) {
			return ::GetWindowLongPtrA(hWnd, nIndex);
		}
		// ::SetWindowLongPtr
		inline static LONG_PTR set_window_long_ptr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
			return ::SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
		}

	};

	template<>
	struct window_traits<wchar_t> {
		// WNDCLASSEX
		typedef WNDCLASSEXW wnd_class;

		// ::RegisterClassEx
		inline static ATOM register_class(wnd_class *wc) {
			return ::RegisterClassExW(wc);
		}

		// ::CreateWindowEx
		inline static HWND create_window(
			DWORD dwExStyle,
			LPCWSTR lpClassName,
			LPCWSTR lpWindowName,
			DWORD dwStyle,
			int x,
			int y,
			int nWidth,
			int nHeight,
			HWND hWndParent,
			HMENU hMenu,
			HINSTANCE hInstance,
			LPVOID lpParam
		) {
			return ::CreateWindowExW(
				dwExStyle,
				lpClassName,
				lpWindowName,
				dwStyle,
				x, y, nWidth, nHeight,
				hWndParent,
				hMenu,
				hInstance,
				lpParam
			);
		}

		// ::DefWindowProc
		inline static LRESULT def_window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
			return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
		}

		// ::GetWindowLongPtr
		inline static LONG_PTR get_window_long_ptr(HWND hWnd, int nIndex) {
			return ::GetWindowLongPtrW(hWnd, nIndex);
		}
		// ::SetWindowLongPtr
		inline static LONG_PTR set_window_long_ptr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
			return ::SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
		}

	};

}}}// namespace lulib::win32api::window
