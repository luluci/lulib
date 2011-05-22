#pragma once
#pragma warning(disable : 4819)

namespace lulib { namespace c_str {

	// buf へ src をコピーする
	// buf のサイズは推論、src はNULL終端である必要がある
	template<typename Char, int N>
	int print(Char (&buf)[N], Char const* src) {
		int i = 0;
		while (i < N-1 && src[i] != '\0') {
			buf[i] = src[i];
			++i;
		}
		buf[i] = '\0';
		return i;
	}
	// buf のサイズは推論、src のサイズは指定
	template<typename Char, std::size_t N>
	int print(Char (&buf)[N], Char const* src, int size) {
		int i = 0;
		while (i < N-1 && i < size) {
			buf[i] = src[i];
			++i;
		}
		buf[i] = '\0';
		return i;
	}
	// buf のサイズは指定、src のサイズは指定
	template<typename Char>
	int print(Char *buf, int size1, Char const* src, int size2) {
		int i = 0;
		while (i < size1-1 && i < size2) {
			buf[i] = src[i];
			++i;
		}
		buf[i] = '\0';
		return i;
	}

}}// namespace lulib::c_str
