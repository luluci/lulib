#pragma once

#include <string>

namespace lulib {

	class path {
		typedef path self_type;

	public:
		path(char const* p) : path_(p) {
		}
		path(std::string const& p) : path_(p) {
		}
		path(std::string && p) : path_( std::move(p) ) {
		}

	public:
		operator std::string const&() { return path_; }
		std::string const& str() const { return path_; }

		self_type parent_path() {
			int len = path_.size();

			// セパレータが現れない間ループ
			auto it = path_.rbegin(), end = path_.rend();
			while (it != end) {
				--len;
				// セパレータを発見したら、セパレータの分まで長さを除去して終了
				if (is_sep(*it)) break;
				++it;
			}

			return self_type( path_.substr(0,len) );
		}

	private:
		inline bool is_sep(char ch) {
			switch (ch) {
			case '/':
			case '\\':
				return true;
			}
			return false;
		}

	private:
		std::string path_;  // 与えられた文字列
	};

}// namespace lulib
