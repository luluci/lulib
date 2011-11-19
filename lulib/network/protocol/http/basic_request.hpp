#pragma once
#pragma warning(disable : 4819)

#include <lulib/network/uri.hpp>
#include <lulib/network/scheme.hpp>
#include <lulib/network/protocol/http/basic_header.hpp>
#include <lulib/network/protocol/http/basic_query.hpp>
#include <lulib/network/protocol/http/version.hpp>
#include <lulib/network/protocol/http/traits/method.hpp>
#include <lulib/network/protocol/http/detail/header_container.hpp>
#include <lulib/network/protocol/http/detail/query_container.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network { namespace http {

	// HTTP request class
	// 
	template<typename Char>
	class basic_request {
		typedef basic_request<Char> self_type;
		typedef lulib::network::uri uri_type;

	public:
		// CharTraits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

	private:
		// HeaderContainer
		typedef typename detail::header_container<Char>::type header_container;
		// QueryContainer
		typedef typename detail::query_container<Char>::type query_container;

		typedef basic_scheme<Char> scheme;


	public:
		template<typename URL>
		basic_request(URL &&url) {
			uri_type uri(url);
			scheme_name = uri.scheme();
			host = uri.host();
			port = uri.port();
			file = uri.file();
			scheme_id = scheme::get_id(scheme_name);
			major_ver_ = 1;
			minor_ver_ = 1;
		}
		template<typename Scheme, typename Host, typename File>
		basic_request(Scheme &&scheme, Host &&host, File &&file, std::size_t port = 0)
		: scheme_name(std::forward<Scheme>(scheme))
		, host(std::forward<Host>(host))
		, file(std::forward<File>(file))
		, port(port)
		, major_ver_(1), minor_ver_(1)
		{
			scheme_id = scheme::get_id(scheme_name);
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

		template<typename P, typename C> friend class basic_client;

		string_type scheme_name;        // スキーム
		typename scheme::enum_t scheme_id;  // スキームID
		string_type host;          // ホスト
		std::size_t port;          // ポート
		string_type file;          // ファイル
		std::size_t major_ver_;    // HTTP major version
		std::size_t minor_ver_;    // HTTP minor version
		header_container headers;
		query_container queries;
	};

	namespace request_detail {
		struct GET {};
		struct POST {};

		template<typename Method, typename Char>
		struct request_wrapper {
			typedef Method method;
			typedef basic_request<Char> request_type;
			request_type const& request;

			request_wrapper(request_type const& req) : request(req) {}
		};
	}

	// GET情報を付与したrequestを返す
	template<typename Char>
	request_detail::request_wrapper<request_detail::GET,Char> GET(basic_request<Char> const& req) {
		return request_detail::request_wrapper<request_detail::GET,Char>(req);
	}
	// POST情報を付与したrequestを返す
	template<typename Char>
	request_detail::request_wrapper<request_detail::POST,Char> POST(basic_request<Char> const& req) {
		return request_detail::request_wrapper<request_detail::POST,Char>(req);
	}

}}}//namespace lulib::network::http

