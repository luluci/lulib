#pragma once

#include <string>
#include <memory>

#include <lulib/sqlite3/typedefs.hpp>

namespace lulib { namespace sqlite3 { namespace detail {

	// sqlite3デリータ
	struct sqlite3_deleter {
		void operator()(sqlite3_type* db) const {
			::sqlite3_close(db);
		}
	};
	// sqlite3 ハンドラ
	typedef std::unique_ptr<sqlite3_type, sqlite3_deleter> sqlite3_ptr;

	// sqlite3_ptr 生成ヘルパ関数
	sqlite3_ptr get_sqlite3(char const* file) {
		sqlite3_type *db = 0;
		// UTF-8でsqlite3_open
		// UTF-16はsqlite3_open16()
		int ret = ::sqlite3_open(file, &db);
		if (ret == SQLITE_OK) {
			return sqlite3_ptr(db);
		}
		return sqlite3_ptr();
	}
	sqlite3_ptr get_sqlite3(std::string const& file) {
		return get_sqlite3(file.c_str());
	}



	// statementデリータ
	struct statement_deleter {
		void operator()( statement_type* statement ) {
			::sqlite3_finalize(statement);
		}
	};
	// statementハンドラ
	typedef std::unique_ptr< statement_type, statement_deleter > statement_ptr;


}}}// namespace lulib::sqlite3::detail
