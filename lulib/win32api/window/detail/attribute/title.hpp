#pragma once
#pragma warning(disable : 4819)

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace attribute {

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

		inline string_type& operator()() { return title_; }

	private:
		string_type title_;  // タイトルバーに表示されるテキスト
	};

}}}}// namespace lulib::win32api::window_detail::attribute
