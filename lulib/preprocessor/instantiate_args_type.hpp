
#include <lulib/preprocessor/narg.hpp>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>

#define INSTANTIATE_ARGS_TYPE(...) \
BOOST_PP_REPEAT( BOOST_PP_SUB(PP_NARG(__VA_ARGS__), 1), INSTANTIATE, ARGS_TO_SEQ(__VA_ARGS__) ) \
INSTANTIATE_TAIL( BOOST_PP_SUB(PP_NARG(__VA_ARGS__), 1), ARGS_TO_SEQ(__VA_ARGS__) )
#define INSTANTIATE(z, n, seq) BOOST_PP_SEQ_ELEM(n, seq)(), 
#define INSTANTIATE_TAIL(n, seq) BOOST_PP_SEQ_ELEM(n, seq)()

#define ARGS_TO_SEQ(...) BOOST_PP_TUPLE_TO_SEQ( PP_NARG(__VA_ARGS__), (__VA_ARGS__))
