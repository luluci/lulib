#pragma once
#pragma warning(disable : 4819)

namespace lulib {

	template<bool, typename Then, typename Else = void>
	struct type_if {
		typedef Else type;
	};

	template<typename Then, typename Else = void>
	struct type_if<true,Then,Else> {
		typedef Then type;
	};

}// namespace lulib
