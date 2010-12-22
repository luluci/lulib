#pragma once

// 1000でミリ秒
#undef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000

#include <iostream>
#include <ctime>

/*
// sample
typedef lulib::benchmark<3, 3, 2> benchmark;
benchmark bm;
int count = 0;
bm([&](){
	count = 0;
	for (int i=0; i<10000000; i++) {
		count += 1;
	}
});
*/

namespace lulib {

	// Loop回のループをSetセットだけ行う
	// その際、最初のIdle回空回しする
	template<std::size_t Set, std::size_t Loop, std::size_t Idle = 0>
	class benchmark {

	public:
		template<typename Func>
		void operator()(Func const& func) {
			reset();

			// Idle回、空回しする
			for (std::size_t idle = 0; idle < Idle; idle++) {
				func();
			}
			// 計測開始
			for (std::size_t set = 0; set < Set; set++) {
				set_start();

				for (std::size_t loop = 0; loop < Loop; loop++) {
					func();
				}

				set_stop(set);
			}

			finish();
		}

	private:
		void reset() {
			total_ = 0;
		}
		void set_start() {
			start_ = std::clock();
		}
		void set_stop(std::size_t i) {
			end_ = std::clock();
			total_ += (end_ - start_);

			std::cerr << "Set : " << i << std::endl;
			std::cerr << "start (msec) : " << start_ << std::endl;
			std::cerr << "end   (msec) : " << end_ << std::endl;
			std::cerr << "proc time (msec) : " << (end_ - start_) << std::endl;
			std::cerr << std::endl;
		}
		void finish() {
			std::cerr << "Total            (msec) : " << total_ << std::endl;
			std::cerr << "Average per Set  (msec) : " << ((double)total_ / Set) << std::endl;
			std::cerr << "Average per Loop (msec) : " << ((double)total_ / (Set * Loop)) << std::endl;
			std::cerr << std::endl;
		}

		typedef std::clock_t clock_type;
		clock_type start_, end_, total_;
	};

}// namespace lulib
