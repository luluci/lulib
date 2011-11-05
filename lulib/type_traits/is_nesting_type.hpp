#pragma once
#pragma warning(disable : 4819)

// あるクラスがnesting typeとして型nesting_typeを持っているかどうかを判定するメタ関数を生成する
// "LULIB_IS_NESTING_TYPE(value_type)" は "is_nesting_value_type<T>" を生成する。
// "is_nesting_value_type<T>::value" は T が "T::value_type" を持っていると true となる。
#define LULIB_IS_NESTING_TYPE(nesting_type)							\
template<typename T>										\
class is_nesting_##nesting_type {								\
	template<typename U>									\
	static std::true_type  test(typename U::nesting_type* = 0);				\
	template<typename U>									\
	static std::false_type test(...);							\
public:												\
	static const bool value = std::is_same< decltype(test<T>(0)), std::true_type >::value;	\
};

