#include <lulib/singleton/singleton.hpp>

#include <iostream>

#include <boost/pool/detail/singleton.hpp>

template<int i>
class comctrl_init {
public:
	comctrl_init() {
		std::cout << "comctrl_init(" << i << ")" << std::endl;
	}
	~comctrl_init() {
		std::cout << "~comctrl_init(" << i << ")" << std::endl;
	}

	// lulib::singleton<T>::instance() ‚Ì•Ê–¼‚Ý‚½‚¢‚È
	static void init() {
		std::cout << "init()" << std::endl;
		lulib::singleton<comctrl_init>::instance();
	}
};

typedef lulib::singleton< comctrl_init<0> > singleton0;
typedef lulib::singleton< comctrl_init<1> > singleton1;

int main() {
	std::cout << "enter: main()" << std::endl;

	//singleton cm;  //=> error
	singleton0::init();
	singleton1::init();
	//comctrl_init::init();

	std::cout << "leave: main()" << std::endl;
}
