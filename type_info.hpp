#pragma once

#include <string>
#include <typeinfo>

#include <cxxabi.h>
#include <stdlib.h>

template<typename T>
struct type {};

namespace lulib {

	template<typename T>
	class type_info {

	public:
		type_info() : type_( typeid( type<T> ) ) {
			demangle( type_.name() );
		}

		operator std::type_info const& () {
			return type_;
		}

		std::string const& name() {
			return demangle_;
		}

	private:
		void demangle(char const* demangle) {
			int status;
			char * const p = abi::__cxa_demangle(demangle, 0, 0, &status);
			demangle_ = p;
			free(p);
		}

		std::type_info const& type_;
		std::string demangle_;
	};

}//namespace lulib
