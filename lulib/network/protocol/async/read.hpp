#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib { namespace network { namespace protocol { namespace async {

	// io_serviceにasync_readをセットする
	// Finish: 処理終了時にコールバックするファンクタ
	template<typename Protocol, typename Finish>
	void read(Protocol &p, boost::asio::streambuf &response, Finish const& finish) {
		// プロトコルが閉じていたら終了
		if (!p) return;

		boost::asio::async_read( p.socket(), response, boost::asio::transfer_at_least(1),
			[&](boost::system::error_code const& error, std::size_t bytes_transferred) {
				detail::read_handle(error, bytes_transferred, p, response, finish);
			}
		);
	}

	namespace detail {
		template<typename Protocol, typename Finish>
		void read_handle(
			boost::system::error_code const& error,
			std::size_t bytes_transferred,
			Protocol &p,
			boost::asio::streambuf &response,
			Finish const& finish
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
						detail::read_handle(error, bytes_transferred, p, response, finish);
					}
				);
			}
			// 何らかのエラーにより終了
			// HTTPの場合: EOF?
			// HTTPSの場合: shut_down
			// が通知される
			// 取得したところまで返すのでsuccess()して終了
			else {
				finish();
			}
		}
	}

}}}}//namespace lulib::network::protocol::async
