#pragma once
#pragma warning(disable : 4819)

#ifndef NDEBUG
	#include <iostream>
	#define DEBUG_OUTPUT(str) std::cerr << str << std::endl;
#else
	#define DEBUG_OUTPUT(str)
#endif

