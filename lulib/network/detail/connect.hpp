#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/connect.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <lulib/network/detail/error_code.hpp>

namespace lulib { namespace network { namespace detail {

	// connect
	template<typename Client, typename Socket, typename Host, typename Scheme>
	void connect(Client &client, Socket &socket, Host &&host, Scheme &&scheme) {
		typedef typename Client::resolver          resolver_type;
		typedef typename Client::query             query_type;
		typedef typename Client::endpoint_iterator endpoint_iterator;

		// 変数を初期化
		client.resolver_.async_resolve(
			query_type( std::forward<Host>(host) , std::forward<Scheme>(scheme) ),  // query作成
			[&](error_code const& ec, endpoint_iterator it) {
				resolve_handle(ec, it, client, socket);
			}
		);
	}

	// resolver handler
	template<typename EndpointIterator, typename Client, typename Socket>
	void resolve_handle(error_code const& ec, EndpointIterator it, Client &client, Socket &socket) {
		// resolve成功?
		if (!ec) {
			// socketを開く
			client.open_socket(it->endpoint().protocol());
			// endpointの解決を開始
			// connectが接続するまでsocket.async_connect(it)を自動で繰り返す
			boost::asio::async_connect(socket, it,
				[&](error_code const& ec, EndpointIterator it) {
					connect_handle(ec, it, client, socket);
				}
			);
		}
		// resolve失敗?
		else {
			client.connect_failure(ec);
		}
	}

	template<typename EndpointIterator, typename Client, typename Socket>
	void connect_handle(error_code const& ec, EndpointIterator it, Client &client, Socket &socket) {
		// connectに成功した
		if (!ec) {
			client.connect_success(ec);
		}
		// connectに失敗した
		else {
			client.connect_failure(ec);
		}
	}

}}}//namespace lulib::network::detail

