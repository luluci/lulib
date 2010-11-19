#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				// io_serviceにasync_readをセットする
				template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
				void connect(Protocol &p, EndpointIterator &it, Success const& success, Failure const& failure) {
					// プロトコルが閉じていたら終了
					if (!p) return;

					// endpointの解決を開始
					p.socket().async_connect( *it,
						[&](boost::system::error_code const& error) {
							detail::connect_handle(error, p, ++it, success, failure);
						}
					);

				}

				namespace detail {
					template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
					void connect_handle(
						boost::system::error_code const& error,
						Protocol &p,
						EndpointIterator &it,
						Success const& success, Failure const& failure
					) {
						// プロトコルが閉じていたら終了
						if (!p) return;

						// connectに成功した
						if (!error) {
							//p.async_success();
							success();
						}
						// connectに失敗した
						// かつ、endpoint_iteratorが終端に達していないなら、再帰処理
						else if (it != EndpointIterator()) {
							// 失敗した接続を閉じる
							p.socket().close();
							// リトライ
							p.socket().async_connect( *it,
								[&](boost::system::error_code const& error) {
									connect_handle(error, p, ++it, success, failure);
								}
							);
						}
						// 終端に達していたら、失敗
						else {
							//p.async_failure();
							failure();
						}
					}
				}//namespace detail

			}//namespace async
		}//namespace protocol
	}//namespace network
}//namespace lulib
