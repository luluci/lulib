#pragma once
#pragma warning(disable : 4819)

#include <lulib/win32api/window/basic_window_fwd.hpp>
#include <lulib/win32api/window/policy.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window { namespace attribute {

	template<typename Char = TCHAR>
	struct basic_title {
		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

	public:
		basic_title(char_type const* t) : title_(t) {}
		basic_title(string_type && t) : title_( std::move(t) ) {}
		basic_title(string_type const& t) : title_(t) {}

		template<typename T>
		friend basic_window<T>& operator<<(basic_window<T>&, basic_title<T>&&);

	private:
		string_type title_;  // タイトルバーに表示されるテキスト
	};

	// operator<<
	template<typename Char>
	basic_window<Char>& operator<<(basic_window<Char>& wnd, basic_title<Char> &&t) {
		// titleを更新
		wnd.window_name_ = std::move( t.title_ );
		// すでにウィンドウが作成されているなら、titleの更新を適用
		if (wnd.wnd_ptr_) {
			policy<Char>::set_window_text( wnd.wnd_ptr_.get(), wnd.window_name_.c_str() );
		}
		// 終了
		return wnd;
	}

}}}}// namespace lulib::win32api::window::attribute
