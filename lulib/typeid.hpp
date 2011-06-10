#pragma once

#ifdef __GNUC__

#include <lulib/type_info.hpp>

#define typeid(T) lulib::type_info<T>()

#endif
