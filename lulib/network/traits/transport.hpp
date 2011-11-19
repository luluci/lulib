#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/icmp.hpp>

// トランスポート層Traits

namespace lulib { namespace network {

	// トランスポート層
	struct tcp  {};
	struct udp  {};
	struct icmp {};

	template<typename Protocol>
	struct transport;

	// TCP
	template<>
	struct transport<tcp> {
		typedef boost::asio::ip::tcp type;
		typedef type::acceptor acceptor;
		typedef type::endpoint endpoint;
		typedef type::iostream iostream;
		typedef type::no_delay no_delay;
		typedef type::resolver resolver;
		typedef type::socket   socket;
	};

	// UDP
	template<>
	struct transport<udp> {
		typedef boost::asio::ip::udp type;
		typedef type::endpoint endpoint;
		typedef type::resolver resolver;
		typedef type::socket   socket;
	};

	// ICMP
	template<>
	struct transport<icmp> {
		typedef boost::asio::ip::icmp type;
		typedef type::endpoint endpoint;
		typedef type::resolver resolver;
		typedef type::socket   socket;
	};

}}// namespace lulib::network

