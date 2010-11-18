#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				template<typename Protocol>
				void write(Protocol &p, boost::asio::streambuf &request) {
					if (!p) return;

					// io_serviceにasync_writeをセットする
					boost::asio::async_write( p.socket(), request,
						[&](boost::system::error_code const& error, std::size_t bytes_transferred) {
							// writeに成功した
							if (!error) {
								p.async_success();
							}
							// writeに失敗した
							else {
								p.async_failure();
							}
						}
					);
				}

			}
		}//namespace protocol
	}//namespace network
}//namespace lulib
