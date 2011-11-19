#pragma once
#pragma warning(disable : 4819)

#include <lulib/network/protocol/http/basic_client.hpp>
#include <lulib/network/traits/transport.hpp>

namespace lulib { namespace network { namespace http {

	typedef basic_client<tcp, char> client;

}}}//namespace lulib::network::http
