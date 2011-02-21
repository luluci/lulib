#pragma once

#include <string>

#include <lulib/sqlite3/typedefs.hpp>
#include <lulib/sqlite3/data_type.hpp>

namespace lulib { namespace sqlite3 {

	// step()を実行したとき返すrowクラス
	class row {
		// statementへの参照
		statement_type *stmt_;
		// 列の数
		int column_size_;

	public:
		// ctor
		row() : stmt_(0) {
			column_size_ = ::sqlite3_column_count(stmt_);
		}
		row(statement_type *stmt) : stmt_(stmt) {
			column_size_ = ::sqlite3_column_count(stmt_);
		}
		// move ctor
		row(row &&r) : stmt_(r.stmt_) {
		}
	private:
		// コピーはさせない
		// deleted
		row(row const&);
		row& operator=(row const&);
		row& operator=(row &&);

	public:
		// rowが有効か否か
		inline operator bool() const { return stmt_; }

		// rowが持つ行数
		inline int col_size() const {
			return column_size_;
		}
		// col列のデータ型
		inline int type(int col) const {
			return ::sqlite3_column_type(stmt_, col);
		}

		// optional typedef
		typedef boost::optional<const char*> optional_char;
		typedef boost::optional<std::string> optional_str;
		typedef boost::optional<int> optional_int;
		typedef boost::optional<double> optional_double;

		// char* specialize
		optional_char as_text(int col) const {
			if (col >= column_size_ || type(col) == data_type::null) return optional_char();
			return optional_char( (const char*)::sqlite3_column_text(stmt_, col) );
		}
		// std::string specialize
		optional_str as_string(int col) const {
			if (col >= column_size_ || type(col) == data_type::null) return optional_str();
			return optional_str( std::string( (char const*)::sqlite3_column_text(stmt_, col) ) );
		}
		// int specialize
		optional_int as_int(int col) const {
			if (col >= column_size_ || type(col) == data_type::null) return optional_int();
			return optional_int( ::sqlite3_column_int(stmt_, col) );
		}
		// double specialize
		optional_double as_double(int col) const {
			if (col >= column_size_ || type(col) == data_type::null) return optional_double();
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

}}// namespace lulib::sqlite3
