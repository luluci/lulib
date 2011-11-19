#pragma once

namespace lulib { namespace network { namespace http {

	namespace authorize_detail {
		class null_authorizer {

		public:
			std::string& signature() {
			}
		};
	}// namespace lulib::network::http::autorize_detail

	using authorize_detail::null_authorizer;

}}}// namespace lulib::network::http
