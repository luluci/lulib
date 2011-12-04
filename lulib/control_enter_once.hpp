#pragma once

namespace lulib { namespace detail {

	struct control_enter_once {
		template<typename Functor>
		control_enter_once(Functor &&functor) {
			functor();
		}
	};

}}// namespace lulib::detail

#define CTRL_ENTER_ONCE(functor) {                          \
	static detail::control_enter_once temp( functor );  \
}                                                           \

