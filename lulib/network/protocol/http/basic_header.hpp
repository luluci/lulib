#pragma once
#pragma warning(disable : 4819)

#include <lulib/type_traits/char_traits.hpp>

#include <lulib/network/protocol/http/basic_request_fwd.hpp>

namespace lulib { namespace network { namespace http {
	namespace basic_request_detail {

		template<typename Char>
		struct basic_header {

		public:
			// CharTraits
			typedef lulib::char_traits<Char> char_traits;
			typedef typename char_traits::char_type char_type;
			typedef typename char_traits::string_type string_type;

		public:
			template<typename Name, typename Value>
			basic_header(Name &&name, Value &&value)
			: name_(std::forward<Name>(name)), value_(std::forward<Value>(value))
			{
			}

			template<typename T> friend class basic_request;

		private:
			string_type name_;   // field-name
			string_type value_;  // field-value
		};

	}// namespace basic_request_detail

	using basic_request_detail::basic_header;

	typedef basic_header<char> header;
	typedef basic_header<wchar_t> wheader;

}}}//namespace lulib::network::http
