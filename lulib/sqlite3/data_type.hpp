#pragma once

#include <sqlite3.h>

namespace lulib { namespace sqlite3 {

	// SQLite3 data type
	struct data_type {
		enum enum_t {
			integer = SQLITE_INTEGER,  // int
			float_  = SQLITE_FLOAT,    // double
			text    = SQLITE_TEXT,     // char*
			blob    = SQLITE_BLOB,     // void*
			null    = SQLITE_NULL,     // null
		};
	};

} } // namespace lulib::sqlite3
