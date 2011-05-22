
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include <windows.h>

#include <string>
#include <sstream>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/menu.hpp>

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_COMMAND: {
			std::size_t menu_id = LOWORD(wParam);
			std::string message;
			std::stringstream ss;
			ss << "MenuID:" << menu_id;
			message = ss.str();
			MessageBoxA(0, message.c_str(), "check", MB_OK);
			return 0;
		}
		// 「閉じる」ボタンが押された場合などに通知
		case WM_CLOSE: {
			::DestroyWindow(hWnd);
			return 0;
		}
		// DestroyWindowでウィンドウが破棄されたら通知
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {

	// メニュー作成
	typedef lulib::win32api::menu menu;
	typedef std::shared_ptr<menu> menu_ptr;
	menu_ptr m( new menu() );
	// menu0
	*m << menu::string(0, "menu-0");
	*m << menu::defitem(0);
	// menu1
	*m << menu::string(1, "menu-1");
	*m << menu::disabled(1);
	// separator
	*m << menu::separator(2);
	// menu2
	*m << menu::submenu(3, "submenu-2");
	{
		auto sm = m->get_submenu(3);
		*sm << menu::string(4, "menu-2-0");
		*sm << menu::checked(4);
		*sm << menu::separator(5);
		*sm << menu::submenu(6, "submenu-2-1");
		{
			auto sm2 = sm->get_submenu(6);
			*sm2 << menu::string(7, "menu-2-1-0");
			*sm2 << menu::hilite(7);
			*sm2 << menu::string(8, "menu-2-1-1");
			*sm2 << menu::multi(8, menu::states::disabled | menu::states::checked);
		}
		*sm << menu::menu(9, "menu-2-2");
	}
	*m << menu::menu(10, "menu-3");
	*m << menu::hilite(10);

	/*
	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);         // 構造体サイズ
	wc.style         = CS_HREDRAW | CS_VREDRAW;    // クラススタイル
	wc.cbClsExtra    = 0;                          // 補足メモリブロック
	wc.cbWndExtra    = 0;                          //   のサイズ
	wc.hInstance     = hInst;                      // インスタンス
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL,IDC_ARROW); // カーソル
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);   // 背景色
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "test";                 // クラス名
	wc.hIconSm       = NULL;
	// ウィンドウプロシージャ
	//wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	if (!RegisterClassEx(&wc)) return FALSE;       // 登録
	*/
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
	wnd << window::parent(0),
	wnd << window::menu(m);
	wnd << window::title("test------------");
	wnd << window::ex_style(0) << window::style(WS_OVERLAPPEDWINDOW) <<- window::style(WS_MAXIMIZEBOX);
	wnd << window::pos(100, 100, 400, 100);
	wnd.create();
	wnd.show();

	/*
	// ウィンドウの作成
	HWND hWnd = CreateWindowEx(
		0,
		"test",
		"test",
		WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
		100,100,400,100,  // X Y width height
		0,  // 親ウィンドウ
		menu,  // メニューハンドル
		hInst,
		0
	);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, SW_SHOW);
	*/

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
