#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/menu/basic_menu.hpp>

namespace lulib { namespace win32api { namespace menu {

	// Unicode�t���O(TCHAR)�Ɉˑ������f�t�H���g���j���[�^
	typedef basic_menu< ::CreateMenu >            menu;
	typedef basic_menu< ::CreatePopupMenu > popup_menu;

	// �}���`�o�C�g�������j���[�^
	typedef basic_menu< ::CreateMenu,      char >       menu_a;
	typedef basic_menu< ::CreatePopupMenu, char > popup_menu_a;

	// ���C�h�������j���[�^
	typedef basic_menu< ::CreateMenu,      wchar_t >       menu_w;
	typedef basic_menu< ::CreatePopupMenu, wchar_t > popup_menu_w;

}}}// namespace lulib::win32api::menu
