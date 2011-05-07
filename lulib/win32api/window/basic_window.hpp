#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/window_base.hpp>
#include <lulib/win32api/window/detail/basic_window_op.hpp>
#include <lulib/win32api/window/detail/policy.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_window : public window_base<basic_window<Char>, Char> {
			// self_type
			typedef basic_window<Char> self_type;
			// 基底クラス型
			typedef window_base<basic_window<Char>, Char> base_type;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

			// windowポリシー
			typedef ::lulib::win32api::window_detail::policy<Char> policy;

		public:
			basic_window() : base_type() {}

		private:
			basic_window(self_type const&);
			self_type const& operator=(self_type const&);

		public:
			// friend関数
			// ClassNameの操作
			typedef attribute::basic_window_class_name<Char> class_name;
			typedef attribute::basic_window_class_name<Char> cname;
			template<typename T>
			friend basic_window<T>& attribute::operator<<(basic_window<T>&, attribute::basic_window_class_name<T> &&);

		};
	}//namespace lulib::win32api::window_detail

	using window_detail::basic_window;

}}// namespace lulib::win32api
