#pragma once

#include <boost/optional.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/member.hpp>

#include <lulib/type_traits/has_push_back.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/program_option/value.hpp>
#include <lulib/program_option/option.hpp>
#include <lulib/program_option/error.hpp>

namespace lulib { namespace program_option {

	namespace mi = boost::multi_index;

	template<typename Char>
	class basic_option_container {
		// char traits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		// option type
		typedef basic_option<Char> option_type;
		// tag
		struct opt_name {};
		struct opt_ch {};
		struct opt_desc {};
		// option container
		typedef mi::multi_index_container<
			// 要素
			option_type,
			// 並べ方
			mi::indexed_by<
				mi::ordered_unique<
					mi::tag<opt_name>,
					mi::member<option_type, string_type, &option_type::opt_name_>
				>,
				mi::ordered_non_unique<
					mi::tag<opt_ch>,
					mi::member<option_type,   char_type, &option_type::opt_ch_>
				>,
				mi::sequenced<
					mi::tag<opt_desc>
				>
			>
		> container;
		typedef typename container::template index<opt_name>::type container_by_str;
		typedef typename container::template index<opt_ch>::type   container_by_ch;
		typedef typename container::template index<opt_desc>::type container_by_desc;

		// insert result type
		typedef std::pair<typename container::iterator, bool> result_type;

	public:
		basic_option_container() {
		}

		typedef typename container::iterator iterator;
		inline iterator begin() { return c_.begin(); }
		inline iterator end() { return c_.end(); }

		typedef typename container_by_desc::iterator desc_iterator;
		inline desc_iterator desc_begin() { return c_.template get<opt_desc>().begin(); }
		inline desc_iterator desc_end() { return c_.template get<opt_desc>().end(); }

		// exist
		bool exist(string_type const& key) {
			container_by_str const& c = c_.template get<opt_name>();
			auto it = c.find(key);
			if (it == c.end()) return false;
			return *it;
		}
		bool exist(char_type const& key) {
			if (key == 0) return false;
			container_by_ch const& c = c_.template get<opt_ch>();
			auto it = c.find(key);
			if (it == c.end()) return false;
			return *it;
		}

		// find
		// optionalで結果を通知
		typedef option_type value_type;
		typedef boost::optional<value_type const&> find_result;
		find_result find(string_type const& key) {
			container_by_str const& c = c_.template get<opt_name>();
			auto it = c.find(key);
			if (it == c.end()) return find_result();
			return find_result( *it );
		}
		find_result find(char_type const& key) {
			if (key == 0) return find_result();
			container_by_ch const& c = c_.template get<opt_ch>();
			auto it = c.find(key);
			if (it == c.end()) return find_result();
			return find_result( *it );
		}

		// set
		// 指定したkeyのデータを変更する
		template<typename Modi>
		bool set(string_type const& key, Modi const& modi) {
			// 検索
			container_by_str &c = c_.template get<opt_name>();
			auto it = c.find(key);
			// 存在しなければ修了
			if (it == c.end()) return false;
			// 修正する
			return c.modify(it, modi);
		}
		template<typename Modi>
		bool set(char_type const& key, Modi const& modi) {
			if (key == 0) return find_result();
			// 検索
			container_by_ch &c = c_.template get<opt_ch>();
			auto it = c.find(key);
			// 存在しなければ修了
			if (it == c.end()) return false;
			// 修正する
			return c.modify(it, modi);
		}

		// 指定した名前のコマンドライン引数を有効（受け付けるよう）にする
		// push_back()を持つなら
		template<typename T>
		bool enable(
			string_type const& opt_name,
			char_type opt_ch,
			value<T> const& v,
			string_type const& desc,
			typename boost::enable_if< type_traits::has_push_back<T> >::type* = 0
		) {
			// オプションを追加
			result_type result = c_.insert( option_type(opt_name, opt_ch, desc) );

			// 挿入に失敗（すでに指定されていたら）
			if (!result.second) {
				//std::cerr << "--" << opt_name << " is already enabled." << std::endl;
				return false;
			}

			c_.modify(result.first, [&](value_type &val) {
			// boolタイプ指定
				val.template init<T>( v.opt );
			});

			return true;
		}
		// push_back()を持たないなら
		template<typename T>
		bool enable(
			string_type const& opt_name,
			char_type opt_ch,
			value<T> const& v,
			string_type const& desc,
			typename boost::disable_if< type_traits::has_push_back<T> >::type* = 0
		) {
			// オプションを追加
			result_type result = c_.insert( option_type(opt_name, opt_ch, desc) );

			// 挿入に失敗（すでに指定されていたら）
			if (!result.second) {
				//std::cerr << "--" << opt_name << " is already enabled." << std::endl;
				return false;
			}

			c_.modify(result.first, [&](value_type &val) {
			// boolタイプ指定
				val.template init<T>( v.opt );
			});

			return true;
		}
		// 名前だけで有効になるもの
		bool enable(
			string_type const& opt_name,
			char_type opt_ch,
			string_type const& desc
		) {
			result_type result = c_.insert( option_type(opt_name, opt_ch, desc) );

			// 挿入に失敗（すでに指定されていたら）
			if (!result.second) {
				//std::cerr << "--" << opt_name << " is already enabled." << std::endl;
				return false;
			}

			c_.modify(result.first, [&](value_type &val) {
			// boolタイプ指定
				val.is_bool(true);
			});

			return true;
		}

	private:
		container c_;
	};

} } //namespace lulib::program_option
