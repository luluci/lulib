#pragma once
#pragma warning(disable : 4819)

#include <lulib/network/protocol/http/basic_request.hpp>

namespace lulib { namespace network { namespace http {

	typedef basic_request<char> request;
	typedef basic_request<wchar_t> wrequest;

}}}//namespace lulib::network::http
