#pragma once
#pragma warning(disable : 4819)

#include <type_traits>

#include <boost/preprocessor/facilities/expand.hpp>

#include <lulib/preprocessor/unparen.hpp>

#define LULIB_GENERATE_IS_INSTANCE_OF(params, class_name, args)						\
	template<typename T>										\
	class is_instance_of_##class_name {								\
		template< LULIB_PP_UNPAREN(params) >							\
		static std::true_type test(class_name< LULIB_PP_UNPAREN(args) > &&);			\
		static std::false_type test(...);							\
	public:												\
		static const bool value = std::is_same< decltype(test(T())), std::true_type >::value;	\
	};												\


	//static const bool value = decltype( test(T()) )::value;				\

