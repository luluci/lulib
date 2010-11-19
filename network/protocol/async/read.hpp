#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				// io_serviceにasync_readをセットする
				template<typename Protocol, typename Success, typename Failure>
				void read(Protocol &p, boost::asio::streambuf &response, Success const& success, Failure const& failure) {
					// プロトコルが閉じていたら終了
					if (!p) return;

					boost::asio::async_read( p.socket(), response, boost::asio::transfer_at_least(1),
						[&](boost::system::error_code const& error, std::size_t bytes_transferred) {
							detail::read_handle(error, p, response, success, failure);
						}
					);
				}

				namespace detail {
					template<typename Protocol, typename Success, typename Failure>
					void read_handle(
						boost::system::error_code const& error,
						Protocol &p,
						boost::asio::streambuf &response,
						Success const& success, Failure const& failure
					) {
						// プロトコルが閉じていたら終了
						if (!p) return;

						// 取得できたら
						if (!error) {
							// EOFに達するまで繰り返し読み込む
							boost::asio::async_read(
								p.socket(), response, boost::asio::transfer_at_least(1),
								[&](
									boost::system::error_code const& error,
									std::size_t bytes_transferred
								) {
									// 再帰呼び出し
									detail::read_handle(error, p, response, success, failure);
								}
							);
						}
						// EOFに達したので正常に取得できた
						else if (error == boost::asio::error::eof) {
							//p.async_success();
							success();
						}
						// 失敗したので通知して終了
						else {
							//p.async_failure();
							failure();
						}
					}
				}

			}//namespace async
		}//namespace protocol
	}//namespace network
}//namespace lulib
