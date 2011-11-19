#pragma once
#pragma warning(disable : 4819)

#include <cstddef>

namespace lulib { namespace network { namespace http {

	// HTTP request に version情報をセットする
	struct version {
		version(std::size_t maj, std::size_t min) : major_ver(maj), minor_ver(min) {}

		std::size_t major_ver;
		std::size_t minor_ver;
	};

}}}//namespace lulib::network::http

