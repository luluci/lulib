#pragma once
#pragma warning(disable : 4819)

#include <ostream>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network { namespace http { namespace method {

	template<typename Char> struct GET;

	template<>
	struct GET<char> {
		// CharTraits
		typedef lulib::char_traits<char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		// RequestからGETリクエストを作成し、Buffer(boost::asio::streambuf)に与える
		template<typename Request, typename Buffer>
		static void make_request(Request const& request, Buffer &buffer) {
			std::ostream req(&buffer);
			req << "GET " << request.file();
			// クエリがあるなら追記
			if (!request.queries.empty()) {
				auto it = request.queries.begin(), end = request.queries.end();
				req << "?" << it->first << "=" << it->second;
				++it;
				while (it != end) {
					req << "&" << it->first << "=" << it->second;
					++it;
				}
			}

			req << " HTTP/" << request.major_ver() << "." << request.minor_ver() << "\r\n";
			// Hostヘッダ
			req << "Host: " << request.host() << ":" << request.port() << "\r\n";
			// その他ヘッダを設定
			auto it = request.headers.begin(), end = request.headers.end();
			while (it != end) {
				req << it->first << ": " << it->second << "\r\n";
				++it;
			}
			// ヘッダを閉じる
			req << "\r\n";
		}
	};

}}}} //namespace lulib::network::http::method

