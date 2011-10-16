#pragma once

#include <string>
#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h>
#include <stdlib.h>
#endif

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
#ifdef __GNUC__
			int status;
			char * const p = abi::__cxa_demangle(demangle, 0, 0, &status);
			demangle_ = p;
			free(p);
#else
			demangle_ = demangle;
#endif
		}

		std::type_info const& type_;
		std::string demangle_;
	};

}//namespace lulib
