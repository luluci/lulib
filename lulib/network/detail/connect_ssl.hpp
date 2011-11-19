#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/ssl/stream_base.hpp>

#include <lulib/network/detail/error_code.hpp>

namespace lulib { namespace network { namespace detail {

	// connect
	template<typename Client, typename Socket, typename Host, typename Scheme>
	void connect_ssl(Client &client, Socket &socket, Host &&host, Scheme &&scheme) {
		typedef typename Client::resolver          resolver_type;
		typedef typename Client::query             query_type;
		typedef typename Client::endpoint_iterator endpoint_iterator;

		// プロトコルが閉じていたら終了
		if (!client) return;

		// 変数を初期化
		client.resolver_.async_resolve(
			query_type( std::forward<Host>(host) , std::forward<Scheme>(scheme) ),  // query作成
			[&](error_code const& ec, endpoint_iterator it) {
				resolve_ssl_handle(ec, it, client, socket);
			}
		);
	}

	// resolver handler
	template<typename EndpointIterator, typename Client, typename Socket>
	void resolve_ssl_handle(error_code const& ec, EndpointIterator it, Client &client, Socket &socket) {
		// socketが閉じていたら終了
		if (!client) return;

		// resolve成功?
		if (!ec) {
			// endpointの解決を開始
			// connectが接続するまでsocket.async_connect(it)を自動で繰り返す
			boost::asio::async_connect(socket.lowest_layer(), it,
				[&](error_code const& ec, EndpointIterator it) {
					connect_ssl_handle(ec, it, client, socket);
				}
			);
		}
		// resolve失敗?
		else {
			client.connect_failure(ec);
		}
	}

	template<typename EndpointIterator, typename Client, typename Socket>
	void connect_ssl_handle(error_code const& ec, EndpointIterator it, Client &client, Socket &socket) {
		// socketが閉じていたら終了
		if (!client) return;

		// connectに成功した
		if (!ec) {
			// handshakeに移る
			socket.async_handshake( boost::asio::ssl::stream_base::client,
				[&](error_code const& ec) {
					connect_ssl_handshake(ec, client, socket);
				}
			);
		}
		// connectに失敗した
		else {
			client.connect_failure(ec);
		}
	}

	template<typename Client, typename Socket>
	void connect_ssl_handshake(error_code const& ec, Client &client, Socket &socket) {
		// プロトコルが閉じていたら終了
		if (!client) return;

		// handshakeに成功
		if (!ec) {
			client.connect_success(ec);
		}
		// 失敗
		else {
			client.connect_failure(ec);
		}
	}

}}}//namespace lulib::network::detail

