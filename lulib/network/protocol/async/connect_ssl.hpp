#pragma once

namespace lulib { namespace network { namespace protocol { namespace async {

	// io_serviceにasync_readをセットする
	template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
	void connect_ssl(Protocol &p, EndpointIterator &it, Success const& success, Failure const& failure) {
		// プロトコルが閉じていたら終了
		if (!p) return;

		// endpointの解決を開始
		p.socket().lowest_layer().async_connect( *it,
			[&](boost::system::error_code const& error) {
				detail::connect_ssl_handle(error, p, ++it, success, failure);
			}
		);

	}

	namespace detail {
		template<typename Protocol, typename EndpointIterator, typename Success, typename Failure>
		void connect_ssl_handle(
			boost::system::error_code const& error,
			Protocol &p,
			EndpointIterator &it,
			Success const& success, Failure const& failure
		) {
			// プロトコルが閉じていたら終了
			if (!p) return;

			// connectに成功した
			if (!error) {
				// handshakeに移る
				handshake(p, success, failure);
			}
			// connectに失敗した
			// かつ、endpoint_iteratorが終端に達していないなら、再帰処理
			else if (it != EndpointIterator()) {
				// 失敗した接続を閉じる
				p.socket().lowest_layer().close();
				// リトライ
				p.socket().lowest_layer().async_connect( *it,
					[&](boost::system::error_code const& error) {
						connect_ssl_handle(error, p, ++it, success, failure);
					}
				);
			}
			// 終端に達していたら、失敗
			else {
				failure();
			}
		}

		template<typename Protocol, typename Success, typename Failure>
		void handshake(Protocol &p, Success const& success, Failure const& failure) {
			// プロトコルが閉じていたら終了
			if (!p) return;

			p.socket().async_handshake( boost::asio::ssl::stream_base::client,
				[&](boost::system::error_code const& error) {
					// handshakeに成功
					if (!error) {
						success();
					}
					// 失敗
					else {
						failure();
					}
				}
			);

			return;
		}
	}//namespace detail

}}}}//namespace lulib::network::protocol::async
