#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/streambuf.hpp>

#include <lulib/type_traits/char_traits.hpp>

#include <lulib/network/uri.hpp>
#include <lulib/network/scheme.hpp>
#include <lulib/network/protocol/http/basic_header.hpp>
#include <lulib/network/protocol/http/basic_query.hpp>
#include <lulib/network/protocol/http/version.hpp>
#include <lulib/network/protocol/http/traits/method.hpp>
#include <lulib/network/protocol/http/detail/header_container.hpp>
#include <lulib/network/protocol/http/detail/query_container.hpp>

namespace lulib { namespace network { namespace http {
	namespace basic_request_detail {
		// HTTP request class
		template<typename Char>
		class basic_request {
			typedef basic_request<Char> self_type;
			typedef lulib::network::uri uri_type;

		public:
			// CharTraits
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type char_type;
			typedef typename char_traits::string_type string_type;
			// buffer
			typedef boost::asio::streambuf buffer_type;

		private:
			// HeaderContainer
			typedef typename detail::header_container<Char>::type header_container;
			// QueryContainer
			typedef typename detail::query_container<Char>::type query_container;

			typedef basic_scheme<Char> scheme;
			typedef typename scheme::enum_t scheme_type;

		public:
			template<typename URL>
			basic_request(URL &&url) {
				uri_type uri(url);
				scheme_name_ = uri.scheme();
				host_ = uri.host();
				port_ = uri.port();
				file_ = uri.file();
				scheme_id_ = scheme::get_id(scheme_name_);
				major_ver_ = 1;
				minor_ver_ = 1;
			}
			template<typename Scheme, typename Host, typename File>
			basic_request(Scheme &&scheme, Host &&host, File &&file, std::size_t port = 0)
			: scheme_name_(std::forward<Scheme>(scheme))
			, host_(std::forward<Host>(host))
			, file_(std::forward<File>(file))
			, port_(port)
			, major_ver_(1), minor_ver_(1)
			{
				scheme_id_ = scheme::get_id(scheme_name_);
			}

			// Versionの設定
			self_type& operator<<(version const& ver) {
				major_ver_ = ver.major_ver;
				minor_ver_ = ver.minor_ver;
				return *this;
			}
			// Headerの追加
			// 同じキーが存在する場合は上書き
			self_type& operator<<(basic_header<Char> &&header) {
				headers[ std::move(header.name_) ] = std::move(header.value_);
				return *this;
			}
			// Queryの追加
			// 同じキーが存在する場合は上書き
			self_type& operator<<(basic_query<Char> &&query) {
				queries[ std::move(query.name_) ] = std::move(query.value_);
				return *this;
			}

			buffer_type& buffer() { return buffer_; }

		private:
			// helper
			template<typename R> friend R&& GET(R&&);
			template<typename R> friend R&& POST(R&&);
			// method
			template<typename C> friend class method::GET;
			template<typename C> friend class method::POST;
			typedef method::GET<Char>   get_method;
			typedef method::POST<Char> post_method;
			// GETリクエストを作成してbufferに書きこむ
			void get() {
				buffer_.consume( buffer_.size() );         // buffer clear
				get_method::make_request(*this, buffer_);  // make HTTP request
			}
			// POSTリクエストを作成してbufferに書きこむ
			void post() {
				buffer_.consume( buffer_.size() );          // buffer clear
				post_method::make_request(*this, buffer_);  // make HTTP request
			}

		public:
			string_type const& scheme_name() const { return scheme_name_; }
			scheme_type          scheme_id() const { return scheme_id_; }
			string_type const&        host() const { return host_; }
			std::size_t               port() const { return port_; }
			string_type const&        file() const { return file_; }
			std::size_t          major_ver() const { return major_ver_; }
			std::size_t          minor_ver() const { return minor_ver_; }

		private:
			template<typename P, typename C> friend class basic_client;

			buffer_type buffer_;  // request buffer

			string_type scheme_name_;  // スキーム
			scheme_type scheme_id_;    // スキームID
			string_type host_;         // ホスト
			std::size_t port_;         // ポート
			string_type file_;         // ファイル
			std::size_t major_ver_;    // HTTP major version
			std::size_t minor_ver_;    // HTTP minor version
			header_container headers;  // HTTP header
			query_container queries;   // HTTP query
		};

		// GET情報を付与したrequestを返す
		template<typename Request>
		Request&& GET(Request &&req) {
			req.get();
			return std::forward<Request>(req);
		}
		// POST情報を付与したrequestを返す
		template<typename Request>
		Request&& POST(Request &&req) {
			req.post();
			return std::forward<Request>(req);
		}

	}// namespace basic_request_detail

	using basic_request_detail::basic_request;

}}}//namespace lulib::network::http

