#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <lulib/type_traits/has_push_back.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/program_option/option.hpp>
#include <lulib/program_option/error.hpp>

namespace lulib { namespace program_option {

	// helpを表示
	template<typename Char>
	void print_help(
		std::basic_ostream<Char> &ost,
		typename lulib::char_traits<Char>::string_type const& opt_name,
		typename lulib::char_traits<Char>::char_type opt_ch,
		typename lulib::char_traits<Char>::string_type const& descript,
		int len
	);
	// 2値タイプのhelpを表示
	template<typename Char>
	void print_help_bool(
		std::basic_ostream<Char> &ost,
		typename lulib::char_traits<Char>::string_type const& opt_name,
		typename lulib::char_traits<Char>::char_type opt_ch,
		typename lulib::char_traits<Char>::string_type const& descript,
		int len
	);

	template<>
	void print_help<char>(
		std::basic_ostream<char> &ost,
		typename lulib::char_traits<char>::string_type const& opt_name,
		typename lulib::char_traits<char>::char_type opt_ch,
		typename lulib::char_traits<char>::string_type const& descript,
		int len
	) {
		// 1文字が指定されている
		if (opt_ch != 0) {
			ost << "    -" << opt_ch << " [ ";
		}
		// されてない
		else {
			ost << "         ";
		}

		// 文字列を出力
		ost << "--" << opt_name;
		if (opt_ch != 0) {
			ost << std::setw(len - opt_name.size()) << "" << " ] <arg>    : ";
		}
		else {
			ost << std::setw(len - opt_name.size()) << "" << "   <arg>    : ";
		}
		ost << descript << std::endl;
	}

	template<>
	void print_help<wchar_t>(
		std::basic_ostream<wchar_t> &ost,
		typename lulib::char_traits<wchar_t>::string_type const& opt_name,
		typename lulib::char_traits<wchar_t>::char_type opt_ch,
		typename lulib::char_traits<wchar_t>::string_type const& descript,
		int len
	) {
		// 1文字が指定されている
		if (opt_ch != 0) {
			ost << L"    -" << opt_ch << L" [ ";
		}
		// されてない
		else {
			ost << L"         ";
		}

		// 文字列を出力
		ost << L"--" << opt_name;
		if (opt_ch != 0) {
			ost << std::setw(len - opt_name.size()) << L"" << L" ] <arg>    : ";
		}
		else {
			ost << std::setw(len - opt_name.size()) << L"" << L"   <arg>    : ";
		}
		ost << descript << std::endl;
	}



	template<>
	void print_help_bool<char>(
		std::basic_ostream<char> &ost,
		typename lulib::char_traits<char>::string_type const& opt_name,
		typename lulib::char_traits<char>::char_type opt_ch,
		typename lulib::char_traits<char>::string_type const& descript,
		int len
	) {
		// 1文字が指定されている
		if (opt_ch != 0) {
			ost << "    -" << opt_ch << " [ ";
		}
		// されてない
		else {
			ost << "         ";
		}

		// 文字列を出力
		ost << "--" << opt_name;
		if (opt_ch != 0) {
			ost << std::setw(len - opt_name.size()) << "" << " ]          : ";
		}
		else {
			ost << std::setw(len - opt_name.size()) << "" << "            : ";
		}
		ost << descript << std::endl;
	}

	template<>
	void print_help_bool<wchar_t>(
		std::basic_ostream<wchar_t> &ost,
		typename lulib::char_traits<wchar_t>::string_type const& opt_name,
		typename lulib::char_traits<wchar_t>::char_type opt_ch,
		typename lulib::char_traits<wchar_t>::string_type const& descript,
		int len
	) {
		// 1文字が指定されている
		if (opt_ch != 0) {
			ost << L"    -" << opt_ch << L" [ ";
		}
		// されてない
		else {
			ost << L"         ";
		}

		// 文字列を出力
		ost << L"--" << opt_name;
		if (opt_ch != 0) {
			ost << std::setw(len - opt_name.size()) << L"" << L" ]          : ";
		}
		else {
			ost << std::setw(len - opt_name.size()) << L"" << L"            : ";
		}
		ost << descript << std::endl;
	}


} } //namespace lulib::program_option
