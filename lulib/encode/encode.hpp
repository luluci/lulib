#pragma once
#pragma warning(disable : 4819)

#include <lulib/encode/system.hpp>

namespace lulib {

	// エンコーディングインターフェース
	template<typename System, typename String>
	typename System::result_type encode(String &&str) {
		using std::begin;
		using std::end;
		return System::encode( begin(str), end(str) );
	}
	template<typename System, typename T, std::size_t N>
	typename System::result_type encode(T (&str)[N]) {
		using std::begin;
		using std::end;
		return System::encode( begin(str), end(str)-1 );  // '\0'を飛ばす
	}
	template<typename System, typename Iterator>
	typename System::result_type encode(Iterator begin, Iterator end) {
		return System::encode(begin, end);
	}

}//namespace lulib
