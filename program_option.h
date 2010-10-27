#pragma once

/*
//
Cygwin, g++4.5.1

// sample code
lulib::program_option::option args("オプション");
args.enable("help", 0, "print help");  // 値を取らない。flagのみ
args.enable< std::vector<std::string> >("input-file", 'i', "input file");
args.enable< std::string >("output-file", 'o', "output file");
args.enable< int >("size", 's', "size");
args.nomodify("input-file");  // 修飾されていない引数は "input-file" とみなす
args.parse(argc, argv);  // コマンドライン引数解析

if (args.exist("help")) {
	std::cout << "exist -h" << std::endl;
	args.help();  // 受理するコマンドライン引数を一覧表示
}
if (args.exist('i')) {
	std::vector<std::string> &vec = args["input-file"].as< str_vector >();  // 内部に持っているコンテナへの参照を取得
}
if (args.exist('o')) {
	std::string& str = args["output-file"].as<std::string>();
}

*/

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include <lulib/type_traits/has_push_back.h>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>

namespace lulib {
	namespace program_option {
		class option {
			struct value_type {
				boost::any data_;
				bool active_, is_push_back_c_, is_void_;

				value_type() : active_(false), is_push_back_c_(false), is_void_(false), type_(0) {}
				value_type(const value_type& rhs) : type_(rhs.type_ ? rhs.type_->clone() : 0) {
					data_ = rhs.data_;
					active_ = rhs.active_;
					is_push_back_c_ = rhs.is_push_back_c_;
					is_void_ = rhs.is_void_;
				}
				~value_type() {
					delete type_;
				}

				class bad_as : public std::bad_cast {
				public:
					virtual const char * what() const throw() {
						return "lulib::program_option: failed conversion using as<>";
					}
				};
				template<typename T>
				T& as() {
					T* ptr = boost::any_cast<T>(&data_);
					if (!ptr) throw bad_as();
					return *ptr;
				}
				template<typename T>
				operator T&() {
					return boost::any_cast<T>(&data_);
				}

				template<typename T>
				void add(const char* str, typename boost::enable_if< type_traits::has_push_back<T> >::type* = 0) {
					typedef typename T::value_type type;
					type value = boost::lexical_cast<type>(str);
					boost::any_cast<T>(&data_)->push_back(value);
				}
				template<typename T>
				void add(const char* str, typename boost::disable_if< type_traits::has_push_back<T> >::type* = 0) {
					T value = boost::lexical_cast<T>(str);
					data_ = value;
				}

				value_type& operator=(value_type rhs) {
					std::swap(type_, rhs.type_);
					return *this;
				}

				// 型Tでvalue_typeを初期化する
				template<typename T>
				void init() {
					data_ = T();

					if (type_ != 0) delete type_;
					type_ = new holder<T>();
				}
				void set(const char* str) {
					type_->set(*this, str);
				}

				struct holder_base {
					virtual ~holder_base(){}

					virtual void set(value_type& data, const char* str) = 0;

					virtual holder_base* clone() const = 0;
				};

				template<typename T>
				struct holder : holder_base {
					typedef T type;

					virtual void set(value_type& data, const char* str) {
						data.add<T>( str );
					}
					virtual holder_base* clone() const {
						return new holder<T>();
					}
				};

				holder_base *type_;
			};
			typedef std::map<std::string, value_type> container;
			container c_;
			typedef std::map<char, value_type*> ptr_container;
			ptr_container ch_c_;

			typedef std::pair<container::iterator, bool> result_type;
			typedef std::pair<ptr_container::iterator, bool> ch_result_type;

			// 修飾されていない引数をどうするか
			std::string nomodify_;

			typedef std::vector< std::string > help_container;
			help_container help_;

			std::string descript_;

		public:
			option(const std::string& descript) : c_(), ch_c_(), nomodify_(), descript_(descript) {
			}
			~option() {
			}

			bool exist(const std::string& key) {
				container::iterator it = c_.find(key);
				if (it == c_.end()) return false;
				if (!it->second.active_) return false;
				return true;
			}
			bool exist(char key) {
				ptr_container::iterator it = ch_c_.find(key);
				if (it == ch_c_.end()) return false;
				if (!it->second->active_) return false;
				return true;
			}
			value_type& operator[](const std::string& key) {
				container::iterator it = c_.find(key);
				if (it == c_.end()) throw std::out_of_range("not found argument");
				return it->second;
			}

			// 指定した名前のコマンドライン引数を有効（受け付けるよう）にする
			// push_back()を持つなら
			template<typename T>
			void enable(const std::string& key_str, char key_ch, const std::string& descript, typename boost::enable_if< type_traits::has_push_back<T> >::type* = 0) {
				result_type res = c_.insert( std::make_pair(key_str, value_type()) );

				// 挿入に失敗（すでに指定されていたら）
				if (!res.second) {
					std::cerr << "--" << key_str << " is already enabled." << std::endl;
					return;
				}

				res.first->second.init<T>();
				res.first->second.is_push_back_c_ = true;

				// 追加
				if (key_ch != 0) {
					ch_result_type res2 = ch_c_.insert( std::make_pair(key_ch, &(res.first->second)) );
					if (!res2.second) {
						std::cerr << "-" << key_ch << " is already enabled." << std::endl;
						return;
					}
				}

				// helpを追加
				std::string h = "    ";
				if (key_ch != 0) { h += "-"; h += key_ch; h += " <arg>  "; }
				h += "--" + key_str + " <arg>    " + descript;
				help_.push_back(h);
			}
			// push_back()を持たないなら
			template<typename T>
			void enable(const std::string& key_str, char key_ch, const std::string& descript, typename boost::disable_if< type_traits::has_push_back<T> >::type* = 0) {
				result_type res = c_.insert( std::make_pair(key_str, value_type()) );

				// 挿入に失敗（すでに指定されていたら）
				if (!res.second) {
					std::cerr << "--" << key_str << " is already enabled." << std::endl;
					return;
				}

				res.first->second.init<T>();

				// 追加
				if (key_ch != 0) {
					ch_result_type res2 = ch_c_.insert( std::make_pair(key_ch, &(res.first->second)) );
					if (!res2.second) {
						std::cerr << "-" << key_ch << " is already enabled." << std::endl;
						return;
					}
				}

				// helpを追加
				std::string h = "    ";
				if (key_ch != 0) { h += "-"; h += key_ch; h += " <arg>  "; }
				h += "--" + key_str + " <arg>    " + descript;
				help_.push_back(h);
			}
			// 名前だけで有効になるもの
			void enable(const std::string& key_str, char key_ch, const std::string& descript) {
				result_type res = c_.insert( std::make_pair(key_str, value_type()) );

				// 挿入に失敗（すでに指定されていたら）
				if (!res.second) {
					std::cerr << "--" << key_str << " is already enabled." << std::endl;
					return;
				}

				//c_[char_name].data = true;  // 何もしないと、data.type() == typeid(void);
				res.first->second.is_void_ = true;

				// 追加
				if (key_ch != 0) {
					ch_result_type res2 = ch_c_.insert( std::make_pair(key_ch, &(res.first->second)) );
					if (!res2.second) {
						std::cerr << "-" << key_ch << " is already enabled." << std::endl;
						return;
					}
				}

				// helpを追加
				std::string h = "    ";
				if (key_ch != 0) { h += "-"; h += key_ch; h += "  "; }
				h += "--" + key_str + "    " + descript;
				help_.push_back(h);
			}

			// "--?", "-?"で修飾されない引数
			void nomodify(const std::string& str) {
				nomodify_ = str;
			}

			void help() {
				std::cout << descript_ << std::endl;
				help_container::iterator it = help_.begin();
				while (it != help_.end()) {
					std::cout << *it << std::endl;
					it++;
				}
			}

			void parse(int argc, char **argv) {
				// argv[0]はプログラム名
				int i = 1;
				while (i < argc) {
					// 1文字目がハイフンならオプション
					if (argv[i][0] == '-') {
						// 2文字目のハイフンなら、文字列指定引数
						if (argv[i][1] == '-') {
							i = parse_string(std::string(&(argv[i][2])), argc, argv, i);
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

			int parse_char(char key, int argc, char **argv, int index) {
				ptr_container::iterator it = ch_c_.find(key);

				// 無効なキーなら
				if (it == ch_c_.end()) {
					std::cerr << "invalid option: -" << key << std::endl;
				}
				// 有効なキーなら
				else {
					// 値を指定しない引数なら
					if (it->second->is_void_) {
						it->second->active_ = true;
					}
					// 値を指定する引数なら
					else {
						index++;
						// 範囲外になったら
						if (index >= argc) throw std::out_of_range("not found argument");
						it->second->active_ = true;

						// データを追加
						it->second->set(argv[index]);
					}
				}

				return (index + 1);
			}
			int parse_string(const std::string& key, int argc, char **argv, int index) {
				container::iterator it = c_.find(key);

				// 無効なキーなら
				if (it == c_.end()) {
					std::cerr << "invalid option: --" << key << std::endl;
				}
				// 有効なキーなら
				else {
					// 値を指定しない引数なら
					if (it->second.is_void_) {
						it->second.active_ = true;
					}
					// 値を指定する引数なら
					else {
						index++;
						// 範囲外になったら
						if (index >= argc) throw std::out_of_range("not found argument");
						it->second.active_ = true;

						// データを追加
						it->second.set(argv[index]);
					}
				}

				return (index + 1);
			}
		};

	}// namespace lulib::program_option
}// namespace lulib

