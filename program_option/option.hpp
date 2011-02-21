#pragma once

#include <memory>

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

#include <lulib/type_traits/has_push_back.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/program_option/error.hpp>

namespace lulib { namespace program_option {

	template<typename Char>
	class basic_option {
		// char traits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

	public:
		// ctor
		basic_option(string_type const& opt_name, char_type opt_ch, string_type const& desc)
			: opt_name_(opt_name), opt_ch_(opt_ch), desc_(desc), data_(), type_(),
			active_(false), is_push_back_c_(false), is_bool_(false)
		{}
		// copy ctor
		basic_option(basic_option const& rhs)
			: opt_name_(rhs.opt_name_), opt_ch_(rhs.opt_ch_), desc_(rhs.desc_),
			data_(rhs.data_), type_(rhs.type_ ? rhs.type_->clone() : 0),
			active_(rhs.active_), is_push_back_c_(rhs.is_push_back_c_), is_bool_(rhs.is_bool_)
		{}
		// dtor
		~basic_option() {}

		basic_option& operator=(basic_option const& rhs) {
			opt_name_ = rhs.opt_name_;
			opt_ch_ = rhs.opt_ch_;
			data_ = rhs.data_;
			if ( type_ ) type_.reset( rhs.type_->clone() );
			active_ = rhs.active_;
			is_push_back_c_ = rhs.is_push_back_c_;
			is_bool_ = rhs.is_bool_;
			return *this;
		}

		// オプション設定時
		// 型Tでbasic_optionを初期化する
		template<typename T>
		void init(boost::optional<T> const& opt) {
			data_ = opt;
			type_.reset( new holder<T>() );
		}

		// オプション解析時
		// 型Tを保持しているholderを経由して、addを呼ぶ
		// strを必要に応じてlexical_castしてセットする
		void set(char_type const* str) {
			type_->set(*this, str);
		}
		// 値をセットする
		// T::push_back() がある: 値に対してpush_backする
		template<typename T>
		void add(
			char_type const* str,
			typename boost::enable_if< type_traits::has_push_back<
				//T
				typename boost::remove_reference<T>::type
			> >::type* = 0
		) {
			typedef typename boost::remove_reference<T>::type::value_type type;
			//type value = boost::lexical_cast<type>(str);
			//boost::any_cast<T>(&data_)->push_back(value);
			type value = boost::lexical_cast<type>(str);
			(*boost::any_cast< boost::optional<T> >(&data_))->push_back( value );
		}
		// T::push_back() がない: 代入する
		template<typename T>
		void add(
			char_type const* str,
			typename boost::disable_if< type_traits::has_push_back<
				//T
				typename boost::remove_reference<T>::type
			> >::type* = 0
		) {
			//T value = boost::lexical_cast<T>(str);
			//data_ = value;
			typedef typename boost::remove_reference<T>::type type;
			T value = boost::lexical_cast<type>(str);
			*boost::any_cast< boost::optional<T> >(&data_) = value;
		}

		template<typename T>
		T const& as() const {
			boost::optional<T> const* ptr = boost::any_cast< boost::optional<T> >(&data_);
			if (!ptr) throw bad_as();
			return **ptr;
		}
		/*
		template<typename T>
		operator T&() {
			return boost::any_cast<T>(&data_);
		}
		*/

		// 何らかの値がセットされている
		inline operator bool() const { return active_; }
		inline void is_active(bool value) { active_ = value; }
		// 複数の値を保持するオプション
		inline bool is_container() const { return is_push_back_c_; }
		inline void is_push_back_c(bool value) { is_push_back_c_ = value; }
		// 値を持たないオプション
		inline bool is_bool() const { return is_bool_; }
		inline void is_bool(bool value) { is_bool_ = value; }

	private:
		// TypeErasure
		// 型を記憶しておく
		struct holder_base {
			virtual ~holder_base() {}
			virtual void set(basic_option& data, char_type const* str) = 0;
			virtual holder_base* clone() const = 0;
		};
		template<typename T>
		struct holder : holder_base {
			typedef T type;

			virtual void set(basic_option& data, char_type const* str) {
				data.add<T>( str );
			}
			virtual holder_base* clone() const {
				return new holder<T>();
			}
		};

	public:
		// オプション
		string_type opt_name_;  // オプション名
		char_type   opt_ch_;    // オプション名(1文字)
		string_type desc_;  // 解説

	private:
		// オプション値
		// boost::optional<T>を格納する
		boost::any data_;


		// 型を記憶するためのTypeErasure
		// boost::optional<T>のTを記憶する
		std::unique_ptr<holder_base> type_;
		// フラグ
		bool active_, is_push_back_c_, is_bool_;
	};

} } //namespace lulib::program_option
