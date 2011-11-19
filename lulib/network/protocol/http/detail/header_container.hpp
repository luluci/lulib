#pragma once

#include <map>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network { namespace http { namespace detail {

	template<typename Char>
	struct header_container {

	public:
		// CharTraits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		typedef std::map<string_type, string_type> type;
	};

}}}}// namespace lulib::network::http::detail
