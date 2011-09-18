#pragma once
#pragma warning(disable : 4819)

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network { namespace uri_detail {

	// URIパーツ
	struct uri_parts {
		// CharTraits
		typedef lulib::char_traits<char> char_traits;
		typedef char_traits::char_type char_type;
		typedef char_traits::string_type string_type;

		string_type scheme;
		string_type user_info;
		string_type host;
		string_type port;
		string_type file;
		string_type query;
		string_type fragment;
	};

}}}// namespace lulib::network::uri_detail
