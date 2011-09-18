#pragma once
#pragma warning(disable : 4819)

#include <lulib/network/uri/detail/uri_parser.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network {

	class uri {

	public:
		// CharTraits
		typedef lulib::char_traits<char> char_traits;
		typedef char_traits::char_type char_type;
		typedef char_traits::string_type string_type;

	public:
		uri() : state_(false) {}
		template<typename String>
		uri(String &&uri) : state_(false), uri_(std::forward<String>(uri)) {
			parse_uri_impl();
		}

		// URIが有効か無効か
		operator bool() { return state_; }

		// 解析
		template<typename Str>
		bool parse(Str &&str) {
			uri_ = std::forward<Str>(str);
			parse_uri_impl();
			return state_;
		}

		// access
		string_type const& raw_uri()   const { return uri_;             }
		string_type const& scheme()    const { return parts_.scheme;    }
		string_type const& user_info() const { return parts_.user_info; }
		string_type const& host()      const { return parts_.host;      }
		string_type const& port()      const { return parts_.port;      }
		string_type const& file()      const { return parts_.file;      }
		string_type const& query()     const { return parts_.query;     }
		string_type const& fragment()  const { return parts_.fragment;  }

	private:
		void parse_uri_impl() {
			if ( auto parts_opt = uri_detail::parse_uri(uri_) ) {
				state_ = true;
				parts_ = std::move( *parts_opt );
			}
			else {
				state_ = false;
			}
		}

	private:
		bool state_;               // URI正否
		string_type uri_;          // URI文字列
		uri_detail::uri_parts parts_;  // 分解したURI
	};

}}// namespace lulib::network
