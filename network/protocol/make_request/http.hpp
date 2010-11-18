#pragma once

#include <lulib/network/protocol/make_request/header_container.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace make_request {

				std::string http_get(
					std::string const& host,
					std::string const& file,
					header_container const& header
				) {
					std::string request;

					// メソッド、ファイルを追記
					request = "GET " + file + " HTTP/1.0\r\n";
					// ホストを追記
					request += "Host: " + host + "\r\n";

					// HTTPヘッダを追記
					header_container::const_iterator it = header.cbegin(), end = header.cend();
					while (it != end) {
						request += it->first + ": " + it->second + "\r\n";
						it++;
					}

					// リクエストを閉じる
					request += "\r\n";

					return request;
				}
				std::string http_post(
					std::string const& host,
					std::string const& file,
					header_container const& header,
					std::string const& post
				) {
					std::string request;

					// メソッド、ファイルを追記
					request = "POST " + file + " HTTP/1.0\r\n";
					// ホストを追記
					request += "Host: " + host + "\r\n";

					// HTTPヘッダを追記
					header_container::const_iterator it = header.cbegin(), end = header.cend();
					while (it != end) {
						request += it->first + ": " + it->second + "\r\n";
						it++;
					}

					// ヘッダを閉じる
					request += "\r\n";

					// POSTデータがあるなら
					if (!post.empty()) {
						// POSTを追記
						request += post;
						// リクエストを閉じる
						request += "\r\n";
					}

					return request;
				}
				std::string http(
					std::string const& method,
					std::string const& host,
					std::string const& file,
					header_container const& header,
					std::string const& post
				) {
					std::string request;

					// メソッド、ファイルを追記
					request = method + " " + file + " HTTP/1.0\r\n";
					// ホストを追記
					request += "Host: " + host + "\r\n";

					// HTTPヘッダを追記
					header_container::const_iterator it = header.cbegin(), end = header.cend();
					while (it != end) {
						request += it->first + ": " + it->second + "\r\n";
						it++;
					}

					// リクエストを閉じる
					request += "\r\n";

					// POSTデータがあるなら
					if (!post.empty()) {
						// POSTを追記
						request += post;
						// リクエストを閉じる
						request += "\r\n";
					}

					return request;
				}

			}// namespace make_request
		}// namespace protocol
	}// namespace network
}// namespace lulib


