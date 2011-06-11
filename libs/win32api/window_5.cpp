
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include <windows.h>

#include <string>
#include <sstream>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/static_text.hpp>

class my_window {
	// ウィンドウ型
	typedef lulib::win32api::window_class window_class;
	typedef lulib::win32api::window window;

	// StaticText型
	typedef lulib::win32api::static_text static_text;

public:
	my_window() {}

	bool create(HINSTANCE hInst) {
		// ウィンドウクラス作成
		window_class wnd_class;
		wnd_class.instance(hInst);
		wnd_class.class_name("my_window_class_1");
		wnd_class.style( window_class::styles::h_redraw | window_class::styles::h_redraw );
		wnd_class.background( (HBRUSH)(COLOR_WINDOW+1) );
		wnd_class.icon(NULL);
		wnd_class.icon_small(NULL);
		wnd_class.cursor( LoadCursor(NULL,IDC_ARROW) );
		if ( !wnd_class.register_class() ) return false;

		//
		using std::placeholders::_1;
		using std::placeholders::_2;
		using std::placeholders::_3;
		using std::placeholders::_4;
		// ウィンドウ作成
		wnd_ << window::class_name(wnd_class);
		wnd_ << window::instance(hInst);
		wnd_ << window::procedure( std::bind(&my_window::WndProc, this, _1, _2, _3, _4) );
		wnd_ << window::parent(0);
		wnd_ << window::title("(null)");
		wnd_ << window::style(WS_OVERLAPPEDWINDOW);
		wnd_ << window::pos(100, 100) << window::size(600, 300);
		wnd_.create();
		wnd_.show();

		text_1_ << static_text::title("static control 1")
			<< static_text::wnd_id(1)
			<< static_text::style(WS_VISIBLE | WS_CHILD | SS_NOTIFY)
			<< static_text::instance(hInst)
			<< static_text::parent(wnd_)
			<< static_text::pos(10, 10, 100, 50);
			//<< static_text::subclass( static_text_proc(text1) )
		text_1_.create();
		text_2_ << static_text::title("static control 2")
			<< static_text::wnd_id(2)
			<< static_text::style(WS_VISIBLE | WS_CHILD)
			<< static_text::instance(hInst)
			<< static_text::parent(wnd_)
			<< static_text::pos(10, 70, 100, 50);
		text_2_.create();

		return true;
	}

private:
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg) {
			case WM_COMMAND: {
				std::size_t menu_id = LOWORD(wParam);
				std::string message;
				std::stringstream ss;
				ss << "MenuID:" << menu_id;
				message = ss.str();
				MessageBoxA(hWnd, message.c_str(), "check", MB_OK);
				return 0;
			}
			case WM_CTLCOLORSTATIC: {
				HDC hDC    = (HDC)wParam;
				HWND hCtrl = (HWND)lParam;
				if (hCtrl == text_1_) {
					::SetBkMode(hDC, TRANSPARENT);	// 背景を透過
					return (LRESULT)::GetStockObject(NULL_BRUSH);
				}
				break;
			}
			case WM_CLOSE: {
				DestroyWindow(hWnd);
				return 0;
			}
			case WM_DESTROY: {
				PostQuitMessage(0);
				return 0;
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

private:
	window wnd_;
	static_text text_1_, text_2_;
};

struct static_text_proc {
	typedef lulib::win32api::static_text stext;
	stext &stext_;
	static_text_proc(stext &st) : stext_(st) {}

	LRESULT operator()(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
			case WM_CTLCOLORSTATIC: {
				::MessageBoxA(0, "WM", "test", 0);
				HDC hdc = ::GetDC( hWnd );
				::SetBkMode(hdc, TRANSPARENT);
				::ReleaseDC(hWnd, hdc);
				return (LRESULT)::GetStockObject(NULL_BRUSH);
			}
		}
		return stext_.default_proc(hWnd, msg, wParam, lParam);
	}
};

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
	my_window wnd;
	wnd.create(hInst);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
