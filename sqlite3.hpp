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
#include <memory>
#include <functional>
#include <iostream>

#include <sqlite3.h>

#include <boost/optional.hpp>

namespace lulib {

	class sqlite3 {
		typedef ::sqlite3 sqlite3_type;
		typedef ::sqlite3_stmt statement_type;

		// sqlite3デリータ
		struct sqlite3_deleter {
			void operator()( ::sqlite3* db) const {
				//std::cout << "destruct: sqlite3*" << std::endl;
				::sqlite3_close(db);
			}
		};
		typedef std::unique_ptr< ::sqlite3, sqlite3_deleter> sqlite3_pointer;
		sqlite3_pointer get_sqlite3(const std::string &file) {
			::sqlite3 *db = 0;
			// UTF-8でsqlite3_open
			// UTF-16はsqlite3_open16()
			int ret = ::sqlite3_open(file.c_str(), &db);
			if (ret == SQLITE_OK) {
				//std::cout << "check0:" << ret << ":" << db << std::endl;
				return sqlite3_pointer(db);
			}
			return sqlite3_pointer();
		}

		// statementデリータ
		struct statement_deleter {
			void operator()( statement_type* statement ) {
				//std::cout << "destruct: statement*" << std::endl;
				::sqlite3_finalize(statement);
			}
		};
		typedef std::unique_ptr< statement_type, statement_deleter > statement_pointer;

		// ステートメントから先にデストラクトされるべき
		// データベースハンドル
		sqlite3_pointer db_;
		// ステートメントハンドル
		statement_pointer stmt_;

		// step()呼び出しの状況
		// result_code::done : 完了している
		// result_code::row  : データを読み出せる状態
		//   初期値: result_code::done
		int step_state_;

	// enum
	public:
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

	public:
		// row proxy class
		// step()を実行したとき、proxyクラスとして返す行クラス
		class row {
			// statementへの参照
			statement_type *stmt_;
			// 列の数
			int column_size_;

		public:
			row() : stmt_(0) {
				column_size_ = ::sqlite3_column_count(stmt_);
			}
			row(statement_type *stmt) : stmt_(stmt) {
				column_size_ = ::sqlite3_column_count(stmt_);
			}

			inline operator bool() const { return stmt_ != 0; }

			// optional typedef
			typedef boost::optional<const char*> optional_char;
			typedef boost::optional<std::string> optional_str;
			typedef boost::optional<int> optional_int;
			typedef boost::optional<double> optional_double;

			// char* specialize
			optional_char as_text(int col) const {
				if (col >= column_size_) return optional_char();
				return optional_char( (const char*)::sqlite3_column_text(stmt_, col) );
			}
			// std::string specialize
			optional_str as_string(int col) const {
				if (col >= column_size_) return optional_str();
				return optional_str( std::string( (const char*)::sqlite3_column_text(stmt_, col) ) );
			}
			// int specialize
			optional_int as_int(int col) const {
				if (col >= column_size_) return optional_int();
				return optional_int( ::sqlite3_column_int(stmt_, col) );
			}
			// double specialize
			optional_double as_double(int col) const {
				if (col >= column_size_) return optional_double();
				return optional_double( ::sqlite3_column_double(stmt_, col) );
			}
			/*
			// char* specialize
			inline const char* as_text(int col) const {
				return (const char*)::sqlite3_column_text(stmt_, col);
			}
			// std::string specialize
			inline std::string as_string(int col) const {
				return std::string( (const char*)::sqlite3_column_text(stmt_, col) );
			}
			// int specialize
			inline int as_int(int col) const {
				return ::sqlite3_column_int(stmt_, col);
			}
			// double specialize
			inline double as_double(int col) const {
				return ::sqlite3_column_double(stmt_, col);
			}
			*/
		};
		// コールバック関数は、このシグネチャである必要がある
		// コールバック関数がtrueを返す間ループを継続
		// コールバック関数がfalseを返したら、ループを抜けて終了
		typedef std::function<bool(const row&)> callback_type;

	public:
		sqlite3() : db_(), stmt_(), step_state_(result_code::done) {
		}
		sqlite3(const std::string &file) : db_( get_sqlite3(file) ), stmt_(), step_state_(result_code::done) {
		}

		inline bool is_open() {
			return static_cast<bool>(db_);
		}

		bool open(const std::string &file) {
			db_ = get_sqlite3(file);
		}

#ifdef _MSC_VER
		sqlite3& execute(const std::string &query) {
			// prepareを行い、statementハンドルを更新
			// 失敗したならfalse
			//std::cout << "check1" << std::endl;
			if (!prepare(query)) {
				step_state_ == result_code::error; //return result_code::error;
				return *this;
			}
			// statementを実行
			step();

			// 自分を返す
			return *this;
		}
#endif
#ifndef _MSC_VER
		// SQLクエリを一括実行する関数群
		//     execute("query", ...).each_row();と使うことを想定
		// 返り値:
		//     result_code::done : クエリの実行が完了している状態
		//     result_code::row  : クエリの結果としてデータを返している(データを読み出せる)状態
		template<typename... Args>
		sqlite3& execute(std::string const& query, const Args&... args) {
			return execute(query.c_str(), args...);
		}
		template<typename... Args>
		sqlite3& execute(char const* query, const Args&... args) {
			// prepareを行い、statementハンドルを更新
			// 失敗したならfalse
			if (!prepare(query)) {
				step_state_ == result_code::error;
				return *this;
			}
			// statementに引数をバインドする
			if (!bind(args...)) {
				step_state_ == result_code::error;
				return *this;
			}
			// statementを実行
			step();
			// 自分を返す
			return *this;
		}
#endif
		int each_row(const callback_type& func) {
			bool ret;

			// この時点で、すでにstep()が(execute()で)一度呼ばれている前提
			// step()の結果、データを返す限りループ
			while (step_state_ == result_code::row) {
				// funcにrowプロキシクラスを渡す
				ret = func( row(stmt_.get()) );

				// 現在のrowについて処理を行ったので、次のstep()
				step();

				// コールバック関数がfalseを返したので終了
				if (!ret) break;
			}

			//std::cout << "each_row():" << step_state_ << std::endl;
			// 現在の状態を返して終了
			return step_state_;
		}
		// step()の結果、statementが正常終了したかどうか
		bool is_done() {
			return (step_state_ == result_code::done);
		}
		bool has_row() {
			return (step_state_ == result_code::row);
		}

		// SQLクエリを手動で、随時実行する関数群
		//   1.prepare() : SQLクエリをセットする
		//   2. : 同じSQLクエリを繰り返し実行する場合、2-1～2-2を繰り返す
		//     2-1.bind()  : SQLクエリの"?"にデータをセットする
		//     2-2.step()  : SQLクエリを実際に実行。データが返るなら必要に応じて処理
		// SQLクエリをセットする
		bool prepare(std::string const& query) {
			return prepare(query.c_str());
		}
		bool prepare(char const* query) {
			::sqlite3_stmt *statement = 0;

			int ret = ::sqlite3_prepare(db_.get(), query, -1, &statement, 0);
			if (ret == SQLITE_OK) {
				stmt_ = statement_pointer(statement);
				return true;
			}
			return false;
		}

#ifdef _MSC_VER
		void reset() {
			// bindの最初にはresetする
			::sqlite3_reset(stmt_.get());
		}
		// 非再帰的bind
		template<typename T>
		bool bind(int idx, T const& value) {
			// valueをbind
			return bind_data_impl(idx, value);
		}
#endif
#ifndef _MSC_VER
		// SQLクエリにデータをバインドする
		template<typename... Args>
		bool bind(const Args&... args) {
			// bindの最初にreset
			::sqlite3_reset(stmt_.get());
			return bind_impl(1, args...);
		}
#endif
		// prepare, bindしたクエリを実行
		int step() {
			step_state_ = ::sqlite3_step(stmt_.get());
			return step_state_;
		}
		// step()の結果のデータを取り出す、プロキシクラスを返す
		row get_row() {
			// step()の結果、データを返していたら
			if (step_state_ == result_code::row) {
				return row(stmt_.get());
			}
			return row();
		}

		// step()がデータを返す場合、
		// プロキシクラスrowをコールバックする
		// ファンクタは、trueを返す間ループする、falseで終了
		int step(const callback_type &func) {
			// result_code::rowを返す間、rowプロキシクラスをコールバック
			while ( (step_state_ = step()) == result_code::row ) {
				if ( !func( row(stmt_.get()) ) ) return step_state_;
			}

			return step_state_;
		}

	private:
		bool bind_impl(int idx) {
			return true;
		}
		template<typename T, typename... Args>
		bool bind_impl(int idx, const T& value, const Args&... args) {

			// valueをbind
			if (!bind_data_impl(idx, value)) return false;
			// 次の引数
			return bind_impl(idx+1, args...);
		}
		template<typename T>
		bool bind_data_impl(int idx, T const& value);
		// std::string受理
		bool bind_data_impl(int idx, std::string const& value) {
			// textとしてbind
			int ret = ::sqlite3_bind_text(stmt_.get(), idx, value.c_str(), -1, 0);
			// bindが成功したか？
			return (ret == SQLITE_OK);
		}
		// char*受理
		bool bind_data_impl(int idx, char const* value) {
			// textとしてbind
			int ret = ::sqlite3_bind_text(stmt_.get(), idx, value, -1, 0);
			// bindが成功したか？
			return (ret == SQLITE_OK);
		}
		// int受理
		bool bind_data_impl(int idx, int const& value) {
			// textとしてbind
			int ret = ::sqlite3_bind_int(stmt_.get(), idx, value);
			// bindが成功したか？
			return (ret == SQLITE_OK);
		}
		// double受理
		bool bind_data_impl(int idx, double const& value) {
			// textとしてbind
			int ret = ::sqlite3_bind_double(stmt_.get(), idx, value);
			// bindが成功したか？
			return (ret == SQLITE_OK);
		}

	};

}
