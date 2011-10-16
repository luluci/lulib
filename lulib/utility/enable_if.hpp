#pragma once

///*
#ifdef _MSC_VER
#    define LULIB_ENABLE_DEFAULT typename Enabler* = 0
#    define LULIB_ENABLE_IF(BOOL) typename std::enable_if<BOOL>::type* = 0
#else
#    define LULIB_ENABLE_DEFAULT typename Enabler*& = lulib::enabler
#    define LULIB_ENABLE_IF(BOOL) typename std::enable_if<BOOL>::type*& = lulib::enabler
#endif
//*/

//#    define LULIB_ENABLE_IF(BOOL) typename std::enable_if<BOOL>::type

namespace lulib {

	// enabler
	extern void * enabler;

}

