#pragma once

# define LULIB_PP_UNPAREN_IMPL(...) __VA_ARGS__ 
# define LULIB_PP_UNPAREN(pack) LULIB_PP_UNPAREN_IMPL pack

