#pragma once

#include <sqlite3.h>

namespace lulib { namespace sqlite3 {

	// SQLite3 result codes
	struct result_code {
		enum enum_t {
			ok = SQLITE_OK,
			error = SQLITE_ERROR,

			// step() result
			row = SQLITE_ROW,    // step()がデータを返している
			done = SQLITE_DONE,  // step()が完了した
		};
	};

} } // namespace lulib::sqlite3
