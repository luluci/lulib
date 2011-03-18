#pragma once

#include <string>
#include <functional>

#include <boost/optional.hpp>

#include <sqlite3.h>

#include <lulib/sqlite3/row.hpp>
#include <lulib/sqlite3/typedefs.hpp>
#include <lulib/sqlite3/result_code.hpp>
#include <lulib/sqlite3/detail/handler.hpp>

namespace lulib { namespace sqlite3 {

	class statement {
		typedef lulib::sqlite3::statement_type statement_type;
		// ステートメントハンドル
		typedef detail::statement_ptr statement_ptr;
		statement_ptr stmt_;

		// step()呼び出しの状況
		// result_code::done : 完了している
		// result_code::row  : データを読み出せる状態
		//   初期値: result_code::done
		int step_state_;

		// エラーコード
		int error_code_;

	public:
		// ctor
		statement(sqlite3_type *db, char const* query, int query_size = -1)
			: stmt_(), step_state_(result_code::done)
		{
			prepare(db, query, query_size);
		}
		// move ctor
		statement(statement && s)
			: stmt_( std::move(s.stmt_) ),
			step_state_(s.step_state_),
			error_code_(s.error_code_)
		{
		}

	private:
		// deleted
		statement(statement const&);
		statement& operator=(statement const&);

	public:
		inline operator bool() {
			return static_cast<bool>(stmt_);
		}
		inline bool is_open() {
			return static_cast<bool>(stmt_);
		}

		// step()が完了しているかどうか
		bool is_done() {
			return (step_state_ == result_code::done);
		}
		// step()がrowを返しているかどうか
		bool has_row() {
			return (step_state_ == result_code::row);
		}

		// SQLクエリを実行する関数群
		//   1. prepare() : SQLクエリをセットする
		//   2. : 同じSQLクエリを繰り返し実行する場合、2-1～2-3を繰り返す
		//     2-1. bind()  : SQLクエリの"?"にデータをセットする
		//     2-2. step()  : SQLクエリを実際に実行。データが返るなら必要に応じて処理
		//     2-3. reset() : ステートメントを再実行可能にする。
		//                    bind_*()でバインドされたデータはクリアされない

#ifdef _MSC_VER
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

		// ステートメントを再実行可能にする
		void reset() {
			// bindの最初にはresetする
			::sqlite3_reset(stmt_.get());
		}

		// prepare, bindしたクエリを実行(step())
		// step()の結果、データが返されたらrowを返す
		row step() {
			step_state_ = ::sqlite3_step(stmt_.get());
			return get_row();
		}

		// step()の結果のデータを取り出すrowクラスを返す
		row get_row() {
			// step()の結果、データを返していたら
			if (has_row()) return row(stmt_.get());
			else return row();
		}

		// コールバック関数は、このシグネチャである必要がある
		// コールバック関数がtrueを返す間ループを継続
		// コールバック関数がfalseを返したら、ループを抜けて終了
		typedef std::function<bool(row const&)> callback_type;

		// step()がデータを返す場合、
		// プロキシクラスrowをコールバックする
		// ファンクタは、trueを返す間ループする、falseで終了
		bool each(callback_type const& func) {
			// result_code::rowを返す間、rowプロキシクラスをコールバック
			while ( row r = step() ) {
				if ( !func(r) ) return false;
			}
			// ループ全完了
			return true;
		}

	private:
		// SQLクエリをセットする
		bool prepare(sqlite3_type *db, std::string const& query, int query_size) {
			return prepare(db, query.c_str(), query_size);
		}
		bool prepare(sqlite3_type *db, char const* query, int query_size) {
			statement_type *stmt = 0;
			// prepare実行
			error_code_ = ::sqlite3_prepare(db, query, -1, &stmt, 0);
			stmt_.reset(stmt);
			// prepareの結果を通知
			return error_code_ == result_code::ok;
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

} } // namespace lulib::sqlite3
