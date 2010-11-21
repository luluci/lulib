#pragma once

#include <istream>
#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace parser {

				// HTTPレスポンスを解析して分割する
				template<typename Buffer>
				void http_response(
					Buffer &response,
					unsigned int &status_code,
					std::string &header,
					std::string &body
				) {
					// streamをセット
					std::istream rs(&response);
					// HTTPバージョンを取得
					std::string http_version;
					rs >> http_version;
					// ステータスコードを取得
					rs >> status_code;
					// ステータスメッセージ
					std::string status_message;
					std::getline(rs, status_message);
					if (!rs || http_version.substr(0, 5) != "HTTP/") {
						return;
					}
					if (status_code != 200) {
						return;
					}
					
					// ヘッダを取得
					std::string part;
					while ( std::getline(rs, part) && part != "\r" ) header += part;
					
					// ボディを取得
					std::stringstream ss;
					ss << &response;
					body += ss.str();
				}

			}//namespace parser
		}//namespace protocol
	}//namespace network
}//namespace lulib
