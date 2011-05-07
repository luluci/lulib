
#include <windows.h>

#include <sstream>
#include <functional>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/menu.hpp>
#include <lulib/win32api/list_view.hpp>

// WindowMessage
struct wm {
	// WM_COMMAND�ɑ����郁�b�Z�[�W
	struct command {
		enum enum_t {
			exit,
			edit_list_view,
			lv_add_1,
			lv_delete_2,
		};
	};
};

class my_window {
	// �E�B���h�E�^
	typedef lulib::win32api::window_class window_class;
	typedef lulib::win32api::window window;

	// ���j���[�^
	typedef lulib::win32api::menu menu;
	typedef std::shared_ptr<menu> menu_ptr;

	// ListView�E�B���h�E�^
	typedef lulib::win32api::list_view list_view;

public:
	my_window() : row_count_(0) {}

	bool create(HINSTANCE hInst) {
		// ���j���[�쐬
		menu_.reset( new menu() );
		*menu_ << menu::string(wm::command::exit, "Exit");

		// �^�C�g�����상�j���[
		*menu_ << menu::submenu( wm::command::edit_list_view, "ListView�̑���");
		{
			auto submenu = menu_->get_submenu(wm::command::edit_list_view);
			*submenu
				<< menu::string( wm::command::lv_add_1, "�s�ǉ�")
				//<< menu::string( wm::command::lv_delete_2, "Title-2")
			;
		}

		// �E�B���h�E�N���X�쐬
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
		// �E�B���h�E�쐬
		wnd_ << window::class_name(wnd_class);
		wnd_ << window::instance(hInst);
		wnd_ << window::procedure( std::bind(&my_window::WndProc, this, _1, _2, _3, _4) );
		wnd_ << window::parent(0),
		wnd_ << window::menu(menu_);
		wnd_ << window::title("(null)");
		wnd_ << window::ex_style(0)
			<< window::style(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN)
			<<- window::style(WS_MAXIMIZEBOX);
		wnd_ << window::pos(100, 100) << window::size(400, 100);
		wnd_.create();
		wnd_.show();

		// �N���C�A���g�̈���擾
		RECT rc;
		::GetClientRect(wnd_, &rc);

		// ListView�E�C���h�E�쐬
		lv_ << list_view::instance(hInst);
		lv_ << list_view::parent(wnd_),
		lv_ << list_view::pos(rc);
		lv_ << list_view::style(WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS);
		lv_ << list_view::ex_style(WS_EX_CLIENTEDGE);
		lv_.create();
		lv_.show();
		// ListViewExStyle��ݒ�
		lv_ << list_view::lv_ex_style(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		// ��쐬
		list_view::column col;
		// ��1
		col.text("��1");
		col.width(40);
		col.subitem(1);
		col.index(10);
		lv_ << col;
		// ��2
		col.text("��2");
		col.width(60);
		col.subitem(2);
		col.index(1);
		lv_ << col;

		return true;
	}

private:
	void on_command(std::size_t id) {

		switch (id) {
			case wm::command::exit: {
				::DestroyWindow(wnd_);
				break;
			}
			// Title
			case wm::command::lv_add_1: {
				std::stringstream ss;
				ss << (row_count_+1) << "�s";

				list_view::item item;
				// 1���
				item.pos(row_count_,0);
				item.text(ss.str() + "1��");
				lv_ << item;
				// 2���
				item.pos(row_count_,1);
				item.text(ss.str() + "2��");
				lv_ << item;

				++row_count_;

				break;
			}
		}
	}

	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg) {
			case WM_SIZE: {
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				switch (wParam) {
				case SIZE_MINIMIZED:  // �u�ŏ����v
					break;
		
				case SIZE_RESTORED:  // �ŏ����A�ő剻�ȊO�ŃT�C�Y���ύX���ꂽ
					lv_.resize(0, 0, width, height);
					return 0;
		
				default:
					break;
				}
				break;
			}

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
	window wnd_;
	menu_ptr menu_;
	list_view lv_;
	int row_count_;
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
