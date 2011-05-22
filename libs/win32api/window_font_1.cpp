
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include <windows.h>

#include <string>
#include <sstream>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/dc/paint_struct.hpp>
#include <lulib/win32api/font.hpp>

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_PAINT: {
			// font作成
			typedef lulib::win32api::font font;
			font font_;
			font_ << font::init(hWnd);
			font_ << font::pt(11);
			font_ << font::underline;
			font_ << font::face("MeiryoKe_Gothic");
			font_.create();
			// 描画開始
			lulib::win32api::paint_struct ps(hWnd);

			{
				char const* text = "test テキスト";
				TextOut(ps , 10 , 10 , text , strlen(text));
			}
			{
				ps << font_;
				char const* text = "test テキスト";
				TextOut(ps , 10 , 30 , text , strlen(text));
			}
			return 0;
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

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {

	// ウィンドウクラス
	typedef lulib::win32api::window_class window_class;
	window_class wnd_class;
	wnd_class.instance(hInst);
	wnd_class.class_name("test_window");
	wnd_class.style( window_class::styles::h_redraw | window_class::styles::h_redraw );
	wnd_class.background( (HBRUSH)(COLOR_WINDOW+1) );
	wnd_class.icon(NULL);
	wnd_class.icon_small(NULL);
	wnd_class.cursor( LoadCursor(NULL,IDC_ARROW) );
	if ( !wnd_class.register_class() ) return 1;

	typedef lulib::win32api::window window;
	//
	using std::placeholders::_1;
	using std::placeholders::_2;
	using std::placeholders::_3;
	using std::placeholders::_4;
	//
	window wnd;
	wnd << window::class_name(wnd_class);
	wnd << window::instance(hInst);
	wnd << window::procedure(WndProc);
	wnd << window::parent(0);
	wnd << window::title("test------------");
	wnd << window::ex_style(0) << window::style(WS_OVERLAPPEDWINDOW) <<- window::style(WS_MAXIMIZEBOX);
	wnd << window::pos(100, 100, 400, 100);
	wnd.create();
	wnd.show();

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
