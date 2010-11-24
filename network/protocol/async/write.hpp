#pragma once

#include <boost/asio/streambuf.hpp>

namespace lulib {
	namespace network {
		namespace protocol {
			namespace async {

				template<typename Protocol, typename Success, typename Failure>
				void write(Protocol &p, boost::asio::streambuf &request, Success const& success, Failure const& failure) {
					if (!p) return;

					// io_serviceにasync_writeをセットする
					boost::asio::async_write( p.socket(), request,
						[&](boost::system::error_code const& error, std::size_t bytes_transferred) {
							// writeに成功した
							if (!error) {
								success();
							}
							// writeに失敗した
							else {
								failure();
							}
						}
					);
				}

			}
		}//namespace protocol
	}//namespace network
}//namespace lulib
