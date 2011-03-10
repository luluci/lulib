#pragma once
#pragma warning(disable : 4819)

#include <string>

namespace lulib {
	// char 特性
	template<typename Char>
	struct char_traits {
		typedef Char char_type;
		typedef std::basic_string<Char> string_type;
	};

}// namespace lulib
