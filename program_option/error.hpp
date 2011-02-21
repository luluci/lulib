#pragma once

#include <stdexcept>

namespace lulib { namespace program_option {

	class bad_as : public std::bad_cast {
	public:
		virtual const char * what() const throw() {
			return "lulib::program_option: failed conversion using as<>";
		}
	};


} } //namespace lulib::program_option
