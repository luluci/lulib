#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/connect.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <lulib/network/detail/error_code.hpp>

namespace lulib { namespace network { namespace detail {

	// connect
	template<typename Client, typename Socket, typename Request>
	void connect(Client &client, Socket &socket, Request &request) {
		typedef typename Client::resolver          resolver_type;
		typedef typename Client::query             query_type;
		typedef typename Client::endpoint_iterator endpoint_iterator;

		// 変数を初期化
		client.resolver_.async_resolve(
			query_type( request.host(), request.scheme_name() ),  // query作成
			[&](error_code const& ec, endpoint_iterator it) {
				resolve_handle(ec, it, client, socket, request);
			}
		);
	}

	// resolver handler
	template<typename EndpointIterator, typename Client, typename Socket, typename Request>
	void resolve_handle(
		error_code const& ec, EndpointIterator it,
		Client &client, Socket &socket, Request &request
	) {
		// resolve成功?
		if (!ec) {
			// socketを開く
			client.open_socket(it->endpoint().protocol());
			// endpointの解決を開始
			// connectが接続するまでsocket.async_connect(it)を自動で繰り返す
			boost::asio::async_connect(socket, it,
				[&](error_code const& ec, EndpointIterator it) {
					connect_handle(ec, it, client, socket, request);
				}
			);
		}
		// resolve失敗?
		else {
			client.connect_failure(ec, socket, request);
		}
	}

	template<typename EndpointIterator, typename Client, typename Socket, typename Request>
	void connect_handle(
		error_code const& ec, EndpointIterator it,
		Client &client, Socket &socket, Request &request
	) {
		// connectに成功した
		if (!ec) {
			client.connect_success(ec, socket, request);
		}
		// connectに失敗した
		else {
			client.connect_failure(ec, socket, request);
		}
	}

}}}//namespace lulib::network::detail

