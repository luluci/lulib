#pragma once

#include <lulib/network/basic_client.hpp>
#include <lulib/network/protocol/https.hpp>

namespace lulib {
	namespace network {
		namespace detail {
			typedef protocol::https<> https_protocol;
		}
		typedef basic_client<detail::https_protocol> https_client;
	}//namespace network
}// namespace lulib
