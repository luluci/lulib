#pragma once

#ifdef _MSC_VER
#    define LULIB_ENABLE_DEFAULT typename Enabler* = 0
#    define LULIB_ENABLE_IF(COND)  typename std::enable_if<  COND::value >::type* = 0
#    define LULIB_DISABLE_IF(COND) typename std::enable_if< !COND::value >::type* = 0
#else
#    define LULIB_ENABLE_DEFAULT typename Enabler*& = lulib::enabler
#    define LULIB_ENABLE_IF(COND)  typename std::enable_if<  COND::value >::type*& = lulib::enabler
#    define LULIB_DISABLE_IF(COND) typename std::enable_if< !COND::value >::type*& = lulib::enabler
#endif

namespace lulib {

	// enabler
	extern void * enabler;

}

