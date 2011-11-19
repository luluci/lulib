#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>

#include <lulib/network/detail/error_code.hpp>

namespace lulib { namespace network { namespace detail {

	// io_serviceにasync_readをセットする
	template<typename Client, typename Socket, typename Response>
	void read(Client &client, Socket &socket, Response &response) {
		// プロトコルが閉じていたら終了
		if (!client) return;

		// read
		boost::asio::async_read( socket, response.buffer(), boost::asio::transfer_at_least(1),
			[&](error_code const& ec, std::size_t bytes_transferred) {
				read_handle(ec, bytes_transferred, client, socket, response);
			}
		);
	}

	template<typename Client, typename Socket, typename Response>
	void read_handle(
		error_code const& ec, std::size_t bytes_transferred,
		Client &client, Socket &socket, Response &response
	) {
		// プロトコルが閉じていたら終了
		if (!client) return;

		// 取得できたら
		if (!ec) {
			response.read_success();  // 読み込んだ文のbufferを渡す
			if (response) {  // 読み込みが完了した
				client.read_success(ec);
				return;
			}

			// EOFに達するまで繰り返し読み込む
			boost::asio::async_read(
				socket, response.buffer(), boost::asio::transfer_at_least(1),
				[&](error_code const& ec, std::size_t bytes_transferred) {
					// 再帰呼び出し
					read_handle(ec, bytes_transferred, client, socket, response);
				}
			);
		}
		// 何らかのエラーにより終了
		// HTTPの場合: EOF?
		// HTTPSの場合: shut_down
		// が通知される
		// 取得したところまで返すのでsuccess()して終了
		else if (ec != boost::asio::error::eof) {
			client.read_failure(ec);
		}
		else {
			client.read_success(ec);
		}
	}

}}}//namespace lulib::network::detail

