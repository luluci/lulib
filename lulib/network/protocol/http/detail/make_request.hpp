#pragma once

#include <lulib/network/protocol/make_request/header_container.hpp>

#include <boost/asio/streambuf.hpp>

namespace lulib { namespace network { namespace http { namespace detail {

	template<typename Method>
	void make_request();

	template<typename Buffer>
	void http_get(
		Buffer &request,
		std::string const& host,
		std::string const& file,
		header_container const& header
	) {
		//boost::asio::streambuf request;
		std::ostream request_stream(&request);
		
		// メソッド、ファイルを追記
		request_stream << "GET " << file << " HTTP/1.0\r\n";
		// ホストを追記
		request_stream << "Host: " << host << "\r\n";
		// HTTPヘッダを追記
		header_container::const_iterator it = header.cbegin(), end = header.cend();
		while (it != end) {
			request_stream << it->first << ": " << it->second << "\r\n";
			it++;
		}
		// リクエストを閉じる
		request_stream << "\r\n";
	}
	template<typename Buffer>
	void http_post(
		Buffer &request,
		std::string const& host,
		std::string const& file,
		header_container const& header,
		std::string const& post
	) {
		//boost::asio::streambuf request;
		std::ostream request_stream(&request);

		// メソッド、ファイルを追記
		request_stream << "POST " << file << " HTTP/1.0\r\n";
		// ホストを追記
		request_stream << "Host: " << host << "\r\n";

		// HTTPヘッダを追記
		header_container::const_iterator it = header.cbegin(), end = header.cend();
		while (it != end) {
			request_stream << it->first << ": " << it->second << "\r\n";
			it++;
		}
		// ヘッダを閉じる
		request_stream << "\r\n";

		// POSTデータがあるなら
		if (!post.empty()) {
			// POSTを追記
			request_stream << post;
			// リクエストを閉じる
			request_stream << "\r\n";
		}
		
		//return std::move(request);
	}
	template<typename Buffer>
	void http(
		Buffer &request,
		std::string const& method,
		std::string const& host,
		std::string const& file,
		header_container const& header,
		std::string const& post
	) {
		//boost::asio::streambuf request;
		std::ostream request_stream(&request);

		// メソッド、ファイルを追記
		request_stream << method << " " << file << " HTTP/1.0\r\n";
		// ホストを追記
		request_stream << "Host: " << host << "\r\n";
		// HTTPヘッダを追記
		header_container::const_iterator it = header.cbegin(), end = header.cend();
		while (it != end) {
			request_stream << it->first << ": " << it->second << "\r\n";
			it++;
		}
		// リクエストを閉じる
		request_stream << "\r\n";

		// POSTデータがあるなら
		if (!post.empty()) {
			// POSTを追記
			request_stream << post;
			// リクエストを閉じる
			request_stream << "\r\n";
		}

		//return std::move(request);
	}

}}}}// namespace lulib::network::http::detail


