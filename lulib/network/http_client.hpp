#pragma once

#include <lulib/network/basic_client.hpp>
#include <lulib/network/protocol/http.hpp>

namespace lulib {
	namespace network {
		namespace detail {
			typedef protocol::http http_protocol;
		}
		typedef basic_client<detail::http_protocol> http_client;
	}//namespace network
}// namespace lulib
