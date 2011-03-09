
#include <windows.h>

#include <string>
#include <sstream>

#include <lulib/win32api/menu.hpp>

int WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {

	// ���j���[�쐬
	lulib::win32api::menu_type menu;
	typedef lulib::win32api::menu_type::state state;
	menu.insert_item(0, "menu-0");
	menu.set_defitem(0);
	menu.insert_item(1, "menu-1");
	menu.set_disabled(1);
	menu.insert_separator(2);
	menu.insert_submenu(3, "submenu-2");
	menu.submenu(3)->insert_item(4, "menu-2-0");
	menu.set_checked(4);
	menu.submenu(3)->insert_separator(5);
	menu.submenu(3)->insert_submenu(6, "submenu-2-1");
	menu.submenu(3)->submenu(6)->insert_item(7, "menu-2-1-0");
	menu.set_hilite(7);
	menu.submenu(3)->submenu(6)->insert_item(8, "menu-2-1-1");
	menu.set_state( 8, state::disabled | state::checked );
	menu.submenu(3)->insert_item(9, "menu-2-2");
	menu.insert_item(10, "menu-3");
	menu.set_hilite(10);

	LRESULT (*proc)(HWND, UINT, WPARAM, LPARAM) = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
		switch(msg) {
			/*
			case WM_MENUSELECT: {
				std::size_t menu_id = LOWORD(wParam);
				std::size_t menu_flag = HIWORD(wParam);
				HMENU hMenu = (HMENU)lParam;
				std::string message;
				std::stringstream ss;
				ss << std::hex;
				ss << "MenuID:" << menu_id << ", MenuFlag:" << menu_flag << ", hMenu:" << hMenu;
				message = ss.str();
				MessageBoxA(0, message.c_str(), "check", MB_OK);

				if (menu_id == 3) break;
				break;

				return 0;
			}
			*/
			case WM_COMMAND: {
				std::size_t menu_id = LOWORD(wParam);
				std::string message;
				std::stringstream ss;
				ss << "MenuID:" << menu_id;
				message = ss.str();
				MessageBoxA(0, message.c_str(), "check", MB_OK);
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
	};

	WNDCLASSEX wc;
	wc.cbSize        = sizeof(WNDCLASSEX);         // �\���̃T�C�Y
	wc.style         = CS_HREDRAW | CS_VREDRAW;    // �N���X�X�^�C��
	wc.cbClsExtra    = 0;                          // �⑫�������u���b�N
	wc.cbWndExtra    = 0;                          //   �̃T�C�Y
	wc.hInstance     = hInst;                      // �C���X�^���X
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL,IDC_ARROW); // �J�[�\��
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);   // �w�i�F
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "test";                 // �N���X��
	wc.hIconSm       = NULL;
	// �E�B���h�E�v���V�[�W��
	//wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.lpfnWndProc = (WNDPROC)proc;
	if (!RegisterClassEx(&wc)) return FALSE;       // �o�^

	// �E�B���h�E�̍쐬
	HWND hWnd = CreateWindowEx(
		0,
		"test",
		"test",
		WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
		100,100,400,100,  // X Y width height
		0,  // �e�E�B���h�E
		menu,  // ���j���[�n���h��
		hInst,
		0
	);
	if (!hWnd) return FALSE;

	ShowWindow(hWnd, SW_SHOW);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
