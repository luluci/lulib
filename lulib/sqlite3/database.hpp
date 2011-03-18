#pragma once


/*
//lulib::sqlite3 db(":memory:");
lulib::sqlite3 db("test1.db");
if (!db.is_open()) {
	std::cout << "not open database" << std::endl;
	return 1;
}

db.execute("CREATE TABLE my_table (id INTEGER PRIMARY KEY, name TEXT, place TEXT);");
std::cout << "ret:" << db.is_done() << std::endl;

ret = db.prepare("INSERT INTO my_table (id, name, place) VALUES( ?, ?, ? );");
std::cout << "prepare:" << ret << std::endl;
if (!ret) return 1;

ret = db.bind(1, "name1", "place1");
ret = db.step() == lulib::sqlite3::result_code::done;
std::cout << "step:" << ret << std::endl;
if (!ret) return 1;

ret = db.bind(2, "name2", "place2");
ret = db.step() == lulib::sqlite3::result_code::done;

ret = db.bind(3, "name3", "place3");
ret = db.step() == lulib::sqlite3::result_code::done;

// 読み出し
db.execute("SELECT * FROM my_table WHERE id=?", 2).each_row(
	[](const lulib::sqlite3::row& row) -> bool {
		std::cout << "row:" << std::endl;
		std::cout << "  id   :" << *row.as_int(0) << std::endl;
		std::cout << "  name :" << *row.as_text(1) << std::endl;
		std::cout << "  place:" << *row.as_string(2) << std::endl;
		std::cout << std::endl;
		return true;
	}
);

*/

#include <string>

#include <lulib/sqlite3/statement.hpp>
#include <lulib/sqlite3/detail/handler.hpp>

namespace lulib { namespace sqlite3 {

	class database {
		typedef lulib::sqlite3::sqlite3_type sqlite3_type;
		// ステートメントから先にデストラクトされるべき
		// データベースハンドル
		typedef detail::sqlite3_ptr sqlite3_ptr;
		sqlite3_ptr db_;

	public:
		// ctor
		database() : db_() {
		}
		database(char const* file) : db_( detail::get_sqlite3(file) ) {
		}
		database(std::string const& file) : db_( detail::get_sqlite3(file) ) {
		}
		// move ctor
		database(database &&db) : db_( std::move(db.db_) ) {
		}

	private:
		// deleted
		database(database const&);
		database& operator=(database const&);

	public:
		// databaseを開く
		bool open(std::string const& file) {
			db_ = detail::get_sqlite3(file);
			return static_cast<bool>(db_);
		}

		// databaseが開かれているかどうか
		operator bool() {
			return static_cast<bool>(db_);
		}
		inline bool is_open() {
			return static_cast<bool>(db_);
		}

		// SQLクエリからステートメントを作成して返す
		statement prepare(std::string const& query) {
			return statement( db_.get(), query.c_str(), -1 );
		}
		statement prepare(char const* query) {
			return statement( db_.get(), query, -1 );
		}

	};

} } // namespace lulib::sqlite3
