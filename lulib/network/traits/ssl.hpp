#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/ssl.hpp>

#include <lulib/network/traits/transport.hpp>

// SSL Traits

namespace lulib { namespace network {

	template<typename Transport>
	struct ssl : public transport<Transport> {
		typedef transport<Transport> base_type;
		typedef boost::asio::ssl::stream< typename base_type::socket > socket;
		typedef boost::asio::ssl::context context;

		// verify_mode
		typedef boost::asio::ssl::verify_mode verify_mode;
		static const verify_mode verify_none                 = boost::asio::ssl::verify_none;
		static const verify_mode verify_peer                 = boost::asio::ssl::verify_peer;
		static const verify_mode verify_fail_if_no_peer_cert = boost::asio::ssl::verify_fail_if_no_peer_cert;
		static const verify_mode verify_client_once          = boost::asio::ssl::verify_client_once;
	};

}}// namespace lulib::network

