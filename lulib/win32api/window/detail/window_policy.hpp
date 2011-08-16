#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

namespace lulib { namespace win32api { namespace window_detail {

	// Windowポリシー
	template<typename Char>
	struct window_policy;

	template<>
	struct window_policy<char> {
		// WNDCLASSEX
		typedef WNDCLASSEXA wnd_class;

		// ::DefWindowProc
		//inline static LRESULT def_window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		//	return ::DefWindowProcA(hWnd, Msg, wParam, lParam);
		//}
		typedef LRESULT (CALLBACK* wnd_proc)(HWND,UINT,WPARAM,LPARAM);
		static const wnd_proc default_procedure;

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

		// ::GetWindowLongPtr
		inline static LONG_PTR get_window_long_ptr(HWND hWnd, int nIndex) {
			return ::GetWindowLongPtrA(hWnd, nIndex);
		}
		// ::SetWindowLongPtr
		inline static LONG_PTR set_window_long_ptr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
			return ::SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
		}

		// ::GetWindowTextLength
		inline static int get_window_text_length(HWND hWnd) {
			return ::GetWindowTextLengthA(hWnd);
		}
		// ::GetWindowText
		inline static int get_window_text(HWND hWnd, LPSTR string, int size) {
			return ::GetWindowTextA(hWnd, string, size);
		}
		// ::SetWindowText
		inline static BOOL set_window_text(HWND hWnd, LPCSTR string) {
			return ::SetWindowTextA(hWnd, string);
		}

	};
	const window_policy<char>::wnd_proc window_policy<char>::default_procedure = ::DefWindowProcA;

	template<>
	struct window_policy<wchar_t> {
		// WNDCLASSEX
		typedef WNDCLASSEXW wnd_class;

		// ::DefWindowProc
		//inline static LRESULT def_window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		//	return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
		//}
		typedef LRESULT (CALLBACK* wnd_proc)(HWND,UINT,WPARAM,LPARAM);
		static const wnd_proc default_procedure;

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

		// ::GetWindowLongPtr
		inline static LONG_PTR get_window_long_ptr(HWND hWnd, int nIndex) {
			return ::GetWindowLongPtrW(hWnd, nIndex);
		}
		// ::SetWindowLongPtr
		inline static LONG_PTR set_window_long_ptr(HWND hWnd, int nIndex, LONG_PTR dwNewLong) {
			return ::SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
		}

		// ::GetWindowTextLength
		inline static int get_window_text_length(HWND hWnd) {
			return ::GetWindowTextLengthW(hWnd);
		}
		// ::GetWindowText
		inline static int get_window_text(HWND hWnd, LPWSTR string, int size) {
			return ::GetWindowTextW(hWnd, string, size);
		}
		// ::SetWindowText
		inline static BOOL set_window_text(HWND hWnd, LPCWSTR string) {
			return ::SetWindowTextW(hWnd, string);
		}

	};
	const window_policy<wchar_t>::wnd_proc window_policy<wchar_t>::default_procedure = ::DefWindowProcW;

}}}// namespace lulib::win32api::window_detail
