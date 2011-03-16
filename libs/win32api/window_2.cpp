
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include <windows.h>

#include <functional>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/menu.hpp>

// WindowMessage
struct wm {
	// WM_COMMANDに属するメッセージ
	struct command {
		enum enum_t {
			exit,
			submenu_title,
			title_1,
			title_2,
			title_3,
			submenu_pos,
			pos_1,
			pos_2,
			pos_3,
			pos_4,
			submenu_size,
			size_1,
			size_2,
			size_3,
			size_4,
		};
	};
};

class my_window {
	// ウィンドウ型
	typedef lulib::win32api::window::window_class window_class_type;
	typedef lulib::win32api::window::window window_type;

	// メニュー型
	typedef lulib::win32api::menu::menu menu_type;
	typedef std::shared_ptr<menu_type> menu_ptr;

public:
	my_window() {}

	bool create(HINSTANCE hInst) {
		// メニュー作成
		menu_.reset( new menu_type() );
		typedef menu_type::state state;
		menu_->insert_item( wm::command::exit, "Exit");

		// タイトル操作メニュー
		menu_->insert_submenu( wm::command::submenu_title, "タイトルの操作");
		{
			auto submenu = menu_->submenu(wm::command::submenu_title);
			submenu->insert_item( wm::command::title_1, "タイトル-タイプZERO");
			submenu->insert_item( wm::command::title_2, "Title-2");
			submenu->insert_item( wm::command::title_3, "たいとる");
		}

		// ウィンドウ位置メニュー
		menu_->insert_submenu( wm::command::submenu_pos, "ウィンドウ位置の操作");
		{
			auto submenu = menu_->submenu(wm::command::submenu_pos);
			submenu->insert_item( wm::command::pos_1, "右に+50");
			submenu->insert_item( wm::command::pos_2, "左に+50");
			submenu->insert_item( wm::command::pos_3, "下に+50");
			submenu->insert_item( wm::command::pos_4, "(100,100)");
		}

		// ウィンドウサイズメニュー
		menu_->insert_submenu( wm::command::submenu_size, "ウィンドウサイズの操作");
		{
			auto submenu = menu_->submenu(wm::command::submenu_size);
			submenu->insert_item( wm::command::size_1, "幅+50");
			submenu->insert_item( wm::command::size_2, "幅-50");
			submenu->insert_item( wm::command::size_3, "高さ+50");
			submenu->insert_item( wm::command::size_4, "(400,100)");
		}

		// ウィンドウクラス作成
		window_class_type wnd_class;
		wnd_class.instance(hInst);
		wnd_class.class_name("my_window_class_1");
		wnd_class.style( window_class_type::styles::h_redraw | window_class_type::styles::h_redraw );
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
		namespace window = lulib::win32api::window;
		// ウィンドウ作成
		wnd_.class_name(wnd_class);
		wnd_.instance(hInst);
		wnd_.procedure( std::bind(&my_window::WndProc, this, _1, _2, _3, _4) );
		wnd_.parent(0),
		wnd_.menu(menu_);
		wnd_ << window::title("(null)");
		wnd_ << window::ex_style(0) << window::style(WS_OVERLAPPEDWINDOW) <<- window::style(WS_MAXIMIZEBOX);
		wnd_ << window::pos(100, 100) << window::size(400, 100);
		wnd_.create();
		wnd_.show();

		return true;
	}

private:
	void on_command(std::size_t id) {
		namespace window = lulib::win32api::window;

		switch (id) {
			case wm::command::exit: {
				::DestroyWindow(wnd_);
				break;
			}
			// Title
			case wm::command::title_1: {
				wnd_ << window::title("タイトル-タイプZERO");
				menu_->set_checked(id);
				menu_->set_enable(wm::command::title_2);
				menu_->set_enable(wm::command::title_3);
				break;
			}
			case wm::command::title_2: {
				wnd_ << window::title("Title-2");
				menu_->set_checked(id);
				menu_->set_enable(wm::command::title_1);
				menu_->set_enable(wm::command::title_3);
				break;
			}
			case wm::command::title_3: {
				wnd_ << window::title("たいとる");
				menu_->set_checked(id);
				menu_->set_enable(wm::command::title_1);
				menu_->set_enable(wm::command::title_2);
				break;
			}
			// position
			case wm::command::pos_1: {
				wnd_ <<+ window::pos(50, 0);
				break;
			}
			case wm::command::pos_2: {
				wnd_ <<- window::pos(50, 0);
				break;
			}
			case wm::command::pos_3: {
				wnd_ <<- window::pos(0, -50);
				break;
			}
			case wm::command::pos_4: {
				wnd_ << window::pos(100, 100);
				break;
			}
			// size
			case wm::command::size_1: {
				wnd_ <<+ window::size(50, 0);
				break;
			}
			case wm::command::size_2: {
				wnd_ <<- window::size(50, 0);
				break;
			}
			case wm::command::size_3: {
				wnd_ <<- window::size(0, -50);
				break;
			}
			case wm::command::size_4: {
				wnd_ << window::size(400, 100);
				break;
			}
		}
	}

	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg) {
			case WM_COMMAND: {
				std::size_t id = LOWORD(wParam);
				on_command(id);
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

private:
	window_type wnd_;
	menu_ptr menu_;
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
