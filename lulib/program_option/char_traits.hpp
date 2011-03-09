#pragma once

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace program_option {

	template<typename Char>
	struct char_traits : lulib::char_traits<Char> {
	};

} } //namespace lulib::program_option
