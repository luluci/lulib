#pragma once
#pragma warning(disable : 4819)

#include <sstream>

#include <windows.h>
#include <commctrl.h>

#include <lulib/singleton/singleton.hpp>

#include <lulib/win32api/exceptions.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	namespace detail {
		// コモンコントロールクラスのロード
		// Singletonで実装
		template<DWORD icc_class>
		class init_common_control_ex {
			typedef init_common_control_ex<icc_class> self_type;

		public:
			// インスタンス化時にコモンコントロール初期化
			init_common_control_ex() {
				// コモンコントロールの初期化
				INITCOMMONCONTROLSEX icc;
				icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
				icc.dwICC  = icc_class;
				BOOL ret = InitCommonControlsEx(&icc);
				// 初期化失敗
				if (ret == FALSE) {
					std::stringstream ss;
					ss << "Failed to InitCommonControlsEx(" << icc_class << ")";
					throw lulib::win32api::ra_error(ss.str());
				}
			}

		public:
			// Singleton用インターフェース
			// singleton<T>::instance()を呼ぶ
			static void init() {
				lulib::singleton<self_type>::instance();
			}
		};
	}

}}}}// namespace lulib::win32api::window_detail::common_control
