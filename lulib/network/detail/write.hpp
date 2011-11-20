#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/write.hpp>
#include <lulib/network/detail/error_code.hpp>

namespace lulib { namespace network { namespace detail {

	template<typename Client, typename Socket, typename Request>
	void write(Client &client, Socket &socket, Request &request) {
		if (!client) return;

		// io_serviceにasync_writeをセットする
		boost::asio::async_write( socket, request.buffer(),
			[&](error_code const& ec, std::size_t bytes_transferred) {
				// writeに成功した
				if (!ec) {
					client.write_success(ec, bytes_transferred);
				}
				// writeに失敗した
				else {
					client.write_failure(ec, bytes_transferred);
				}
			}
		);
	}

}}}//namespace lulib::network::detail

