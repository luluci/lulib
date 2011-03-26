
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <sstream>
#include <functional>

#include <lulib/win32api/window.hpp>
#include <lulib/win32api/menu.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

// WindowMessage
struct wm {
	// WM_COMMAND�ɑ����郁�b�Z�[�W
	struct command {
		enum enum_t {
			exit,
			action_type1,
			action_type1_1,
			action_type1_2,
			action_type2,
			action_type2_1,
			action_type2_2,
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

	// io service
	typedef boost::asio::io_service io_service;
	// strand
	typedef boost::asio::strand strand;
	// work
	typedef io_service::work work;
	typedef std::unique_ptr<work> work_handler;
	// timer
	typedef boost::asio::deadline_timer timer;

public:
	my_window()
		: ios_()
		, work_( new work(ios_) )
		, strand_1_(ios_), strand_2_(ios_), strand_3_(ios_)
	{}

	~my_window() {
		thread_ios_1_.join();
		thread_ios_2_.join();
		thread_apoptosis_.join();
	}

	bool create(HINSTANCE hInst) {
		// ���j���[�쐬
		menu_.reset( new menu() );
		*menu_ << menu::string(wm::command::exit, "Exit");
		// ���j���[�P
		*menu_ << menu::submenu( wm::command::action_type1, "ActionType1");
		{
			auto submenu = menu_->get_submenu(wm::command::action_type1);
			*submenu
				<< menu::string( wm::command::action_type1_1, "Action1-1")
				<< menu::string( wm::command::action_type1_2, "Action1-2")
			;
		}
		// ���j���[�Q
		*menu_ << menu::submenu( wm::command::action_type2, "ActionType2");
		{
			auto submenu = menu_->get_submenu(wm::command::action_type2);
			*submenu
				<< menu::string( wm::command::action_type2_1, "Action2-1")
				<< menu::string( wm::command::action_type2_2, "Action2-2")
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
		wnd_ << window::ex_style(0) << window::style(WS_OVERLAPPEDWINDOW) <<- window::style(WS_MAXIMIZEBOX);
		wnd_ << window::pos(100, 100) << window::size(400, 100);
		wnd_.create();
		wnd_.show();

		

		return true;
	}

	void run() {
		thread_ios_1_ = boost::thread( boost::bind(&boost::asio::io_service::run, &ios_) );
		thread_ios_2_ = boost::thread( boost::bind(&boost::asio::io_service::run, &ios_) );
		thread_apoptosis_ = boost::thread( boost::bind(&my_window::apoptosis, this) );
	}

private:
	// �܂�A���E
	void apoptosis() {
		for (int i=30; i>=0; i--) {
			std::stringstream ss;
			ss << "Action��t���E�܂ł��Ɓ\�\" << i;
			wnd_ << window::title(ss.str());

			::Sleep(1000);
		}

		wnd_ << window::title("ios�͏I�����܂���");
		// �I��
		work_.reset();
	}

	void on_action1(std::size_t id) {
		std::stringstream ss;
		ss << "ActionType1 (" << id << ")";
		std::string str = ss.str();
		::MessageBoxA(0, str.c_str(), "ActionType1", 0);
		::Sleep(5000);
	}
	void on_action2(std::size_t id) {
		std::stringstream ss;
		ss << "ActionType2 (" << id << ")";
		std::string str = ss.str();
		::MessageBoxA(0, str.c_str(), "ActionType2", 0);
		::Sleep(1000);
	}
	void on_command(std::size_t id) {

		switch (id) {
			case wm::command::exit: {
				//::DestroyWindow(wnd_);  // �E�B���h�E�n���h�����쐬�����X���b�h�ƕʂ̃X���b�h����ł�destroy�ł��Ȃ�
				::SendMessage(wnd_, WM_CLOSE, 0, 0);  // �X���b�h�𒴂��ĂƂ�ł�
				break;
			}
			// Action1
			case wm::command::action_type1_1: {
				ios_.post(strand_2_.wrap(
					boost::bind(&my_window::on_action1, this, 1)
				));
				break;
			}
			case wm::command::action_type1_2: {
				ios_.post(strand_2_.wrap(
					boost::bind(&my_window::on_action1, this, 2)
				));
				break;
			}
			// Action2
			case wm::command::action_type2_1: {
				ios_.post(strand_3_.wrap(
					boost::bind(&my_window::on_action2, this, 1)
				));
				break;
			}
			case wm::command::action_type2_2: {
				ios_.post(strand_3_.wrap(
					boost::bind(&my_window::on_action2, this, 2)
				));
				break;
			}
		}
	}

	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch(msg) {
			case WM_COMMAND: {
				std::size_t id = LOWORD(wParam);
				ios_.post(strand_1_.wrap(
					boost::bind(&my_window::on_command, this, id)
				));
				return 0;
			}
			case WM_CLOSE: {
				::DestroyWindow(hWnd);
				return 0;
			}
			case WM_DESTROY: {
				::PostQuitMessage(0);
				return 0;
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

private:
	window wnd_;
	menu_ptr menu_;

	io_service ios_;
	work_handler work_;  // ���[�v�̃L�[
	// �r������
	// ���L���\�[�X�ɃA�N�Z�X���鏈�����֐��P�ʂŔr����������
	// ���Ȃ肨�������ς�mutex
	strand strand_1_;  // on_command�S��
	strand strand_2_;  // on_action_1�S��
	strand strand_3_;  // on_action_2�S��

	boost::thread thread_ios_1_, thread_ios_2_;
	boost::thread thread_apoptosis_;
};



int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
	my_window wnd;
	wnd.create(hInst);

	wnd.run();

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
