#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <typeinfo>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>

#include <lulib/type_traits/has_push_back.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/program_option/value.hpp>
#include <lulib/program_option/option_container.hpp>
#include <lulib/program_option/print_help.hpp>

namespace lulib {

	template<typename Char>
	class basic_program_option {
		// char traits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		typedef program_option::basic_option_container<Char> container;
		typedef typename container::value_type value_type;

	public:
		basic_program_option(string_type const& descript) : c_(), nomodify_(), descript_(descript) {
		}
		~basic_program_option() {
		}

		// keyが入力されているかどうかを返す
		bool exist(string_type const& key) {
			return c_.exist(key);
		}
		bool exist(char_type key) {
			return c_.exist(key);
		}
		value_type const& operator[](string_type const& key) {
			auto result = c_.find(key);
			if (!result) throw std::out_of_range("not found argument");
			return *result;
		}

		// 指定した名前のコマンドライン引数を有効（受け付けるよう）にする
		template<typename T>
		bool enable(
			string_type const& opt_name,
			program_option::value<T> const& v,
			string_type const& desc
		) {
			return c_.enable(opt_name, 0, v, desc);
		}
		template<typename T>
		bool enable(
			string_type const& opt_name,
			char_type opt_ch,
			program_option::value<T> const& v,
			string_type const& desc
		) {
			return c_.enable(opt_name, opt_ch, v, desc);
		}
		// 名前だけで有効になるもの
		bool enable(string_type const& opt_name, string_type const& desc) {
			return c_.enable(opt_name, 0, desc);
		}
		bool enable(string_type const& opt_name, char_type opt_ch, string_type const& desc) {
			return c_.enable(opt_name, opt_ch, desc);
		}

		// "--?", "-?"で修飾されない引数
		void nomodify(string_type const& str) {
			nomodify_ = str;
		}

		void parse(int argc, char_type **argv) {
			// argv[0]はプログラム名
			int i = 1;
			while (i < argc) {
				// 1文字目がハイフンならオプション
				if (argv[i][0] == '-') {
					// 2文字目のハイフンなら、文字列指定引数
					if (argv[i][1] == '-') {
						i = parse_string( string_type(&(argv[i][2])), argc, argv, i);
					}
					// それ以外なら、1文字指定引数
					else {
						i = parse_char(argv[i][1], argc, argv, i);
					}
				}
				// 装飾されない引数
				else {
					if (!nomodify_.empty()) {
						parse_string(nomodify_, argc, argv, i-1);
					}
					i++;
				}
			}
		}

	private:
		int parse_char(char_type key, int argc, char_type **argv, int index) {
			bool result = c_.set( key, [&]( typename container::value_type& value ) {
				// 値を指定しない引数なら、なにもしない
				if ( value.is_bool() ) {
				}
				else {
					index++;
					// 範囲外になったら
					if (index >= argc) throw std::out_of_range("not found argument");
					// データを追加
					value.set(argv[index]);
				}

				value.is_active(true);
			});

			// 無効なキーなら
			if (!result) {
				//std::cerr << "invalid option: -" << key << std::endl;
			}

			return (index + 1);
		}
		int parse_string(string_type const& key, int argc, char_type **argv, int index) {
			bool result = c_.set( key, [&]( typename container::value_type& value ) {
				// 値を指定しない引数なら、なにもしない
				if ( value.is_bool() ) {
				}
				else {
					index++;
					// 範囲外になったら
					if (index >= argc) throw std::out_of_range("not found argument");
					// データを追加
					value.set(argv[index]);
				}

				value.is_active(true);
			});

			// 無効なキーなら
			if (!result) {
				//std::cerr << "invalid option: --" << key << std::endl;
			}

			return (index + 1);
		}

		void help(std::basic_ostream<Char>& ost) {
			int max_width = 0;

			auto begin = c_.desc_begin(), end = c_.desc_end();

			// 最長の名前を取得
			auto it = begin;
			while (it != end) {
				if (max_width < it->opt_name_.size()) max_width = it->opt_name_.size();
				++it;
			}

			// helpを出力
			ost << descript_ << std::endl;
			it = begin;
			while (it != end) {
				if (it->is_bool()) {
					program_option::print_help_bool(ost, it->opt_name_, it->opt_ch_, it->desc_, max_width);
				}
				else {
					program_option::print_help(ost, it->opt_name_, it->opt_ch_, it->desc_, max_width);
				}
				++it;
			}
		}

		friend std::basic_ostream<Char>& operator<<(
			std::basic_ostream<Char>& ost,
			basic_program_option<Char> & po
		) {
			po.help(ost);
			return ost;
		}

	private:
		container c_;
		// 修飾されていない引数をどうするか
		string_type nomodify_;
		string_type descript_;

	};

	typedef basic_program_option<char> spoption;
	typedef basic_program_option<wchar_t> wpoption;

	/*
	template<typename Char>
	std::basic_ostream<Char>& operator<<(std::basic_ostream<Char> &ost, basic_program_option<Char> const& po) {
		//return po.operator<<(ost);
		return ost;
	}
	*/

}// namespace lulib

