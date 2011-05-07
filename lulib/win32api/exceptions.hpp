#pragma once

#include <stdexcept>

namespace lulib {
	namespace win32api {

		// リソース確保の失敗
		// Resource Acquisition error
		class ra_error : public std::runtime_error {
		public:
			ra_error(const std::string &what) : std::runtime_error(what) {}
			~ra_error() throw() {}
		};

	}
}
