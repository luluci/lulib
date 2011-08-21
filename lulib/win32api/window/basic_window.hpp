#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/window_base.hpp>
#include <lulib/win32api/window/detail/basic_window_op.hpp>
#include <lulib/win32api/window/detail/window_policy.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api {
	namespace window_detail {

		template<typename Char = TCHAR>
		class basic_window : public window_base<basic_window<Char>, basic_window<Char>, Char> {
			// self_type
			typedef basic_window<Char> self_type;
			// 基底クラス型
			typedef window_base<self_type, self_type, Char> base_type;
			// 最上位派生クラス
			// 大元のクラス
			// どのタイプのウィンドウか
			typedef self_type window_type;

			// Char型特性
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type   char_type;
			typedef typename char_traits::string_type string_type;

		public:
			// windowポリシー
			typedef window_policy<Char> policy;

		public:
			basic_window() : base_type() {
				// DefaultProcedureの設定
				// ウィンドウの場合は ::DefWindowProc(A|W)
				self_type::default_proc_ = policy::default_procedure;
			}

		private:
			basic_window(self_type const&);
			self_type const& operator=(self_type const&);

		public:
			// swap
			void swap(self_type &obj) throw() {
				this->base_type::swap(obj);
			}

		public:
			// friend関数
			// ウィンドウプロシージャの操作
			typedef attribute::procedure procedure;
			typedef attribute::procedure proc;
			template<typename C>
			friend typename basic_window<C>::window_type&
			attribute::operator<<(basic_window<C>&, attribute::procedure &&);
			// ClassNameの操作
			typedef attribute::basic_window_class_name<Char> class_name;
			typedef attribute::basic_window_class_name<Char> cname;
			template<typename T>
			friend typename basic_window<T>::window_type&
			attribute::operator<<(basic_window<T>&, attribute::basic_window_class_name<T> &&);

		};
	}//namespace lulib::win32api::window_detail

	using window_detail::basic_window;

}}// namespace lulib::win32api
