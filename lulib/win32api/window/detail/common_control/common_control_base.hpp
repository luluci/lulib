#pragma once
#pragma warning(disable : 4819)

#include <windows.h>

#include <lulib/win32api/window/detail/window_base.hpp>
#include <lulib/win32api/window/detail/common_control/common_control_op.hpp>
#include <lulib/win32api/window/detail/common_control/subclass.hpp>

#include <lulib/win32api/exceptions.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace win32api { namespace window_detail { namespace common_control {

	// 派生クラスにCommonControlの基本的な機能を提供するクラス
	template<typename Derived, typename Window, typename Char>
	class common_control_base : public window_base< common_control_base<Derived,Window,Char>, Window, Char > {
		// 自分
		typedef common_control_base<Derived, Window, Char> self_type;
		// 基底クラス型
		typedef window_base<self_type, Window, Char> base_type;
		// 最上位派生クラス
		// 大元のクラス
		// どのタイプのウィンドウか
		typedef Window window_type;

		// Char型特性
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type   char_type;
		typedef typename char_traits::string_type string_type;

	protected:
		// WNDPROCハンドラ
		//typedef common_control::WNDPROC_handler<Char> WNDPROC_handler;
		//typedef std::unique_ptr< std::remove_pointer<WNDPROC>::type, WNDPROC_handler> proc_handler;
		typedef WNDPROC_handler<Char> proc_handler;

	protected:
		common_control_base() : base_type(), cc_ex_style_(0) {}

	private:
		common_control_base(self_type const&);
		self_type const& operator=(self_type const&);

#ifndef __GNUC__
		template<typename T, typename U, typename V> friend class window_base;
#else
public:
#endif
		// CreateWindowEx(), SetWindowLongPtr(hWnd,GWLP_USERDATA,this)
		// の実行後に呼ばれる
		void on_create() {
			// プロシージャがセットされていたらサブクラス化
			if (this->proc_) {
				subclassing<self_type,Char>(*this);
			}
			// on_create
			on_create<Derived>();
		}
	private:
		// ウィンドウ作成後の処理を呼び出す
#ifdef __GNUC__
		template< typename T, typename std::enable_if< lulib::type_traits::has_on_create<T>::value >::type *& = lulib::enabler >
		inline void on_create() {
			reinterpret_cast<Derived*>(this)->on_create();
		}
		template< typename T, typename std::enable_if< !lulib::type_traits::has_on_create<T>::value >::type *& = lulib::enabler >
		inline void on_create() {}
#else
		template< typename T >
		inline void on_create(typename std::enable_if< lulib::type_traits::has_on_create<T>::value >::type * = 0) {
			reinterpret_cast<Derived*>(this)->on_create();
		}
		template< typename T >
		inline void on_create(typename std::enable_if< !lulib::type_traits::has_on_create<T>::value >::type * = 0) {}
#endif


	public:
		// swap
		void swap(self_type &obj) throw() {
			this->base_type::swap(obj);
			std::swap( this->cc_ex_style_, obj.cc_ex_style_ );
			std::swap( this->proc_handler_, obj.proc_handler_ );
		}

	public:
		// friend関数
		// CommonControl用 Ex Style の操作
		// ウィンドウ作成後にしか操作は意味が無い
		typedef common_control::ex_style cc_ex_style;
		template<typename D, typename W, typename C>
		friend typename common_control_base<D,W,C>::window_type&
		operator<<(common_control_base<D,W,C>&, ex_style &&);
		// サブクラス化
		// operator<<
		typedef common_control::subclass subclass;
		template<typename D, typename W, typename C>
		friend typename common_control_base<D,W,C>::window_type&
		operator<<(common_control_base<D,W,C>&, subclass&&);
		// subclassing
		template<typename T,typename U> friend void subclassing(T&);

	protected:
		std::size_t cc_ex_style_;    // コモンコントロール用拡張スタイル
		proc_handler proc_handler_;  // コモンコントロールデフォルトプロシージャハンドラ

	};

}}}}// namespace lulib::win32api::window_detail::common_control
