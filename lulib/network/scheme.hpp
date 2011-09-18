#pragma once
#pragma warning(disable : 4819)

#include <boost/spirit/include/qi.hpp>

#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network {

	namespace scheme_detail {
		// Boost.Spirit.Qi
		namespace qi = boost::spirit::qi;
		namespace phx = boost::phoenix;
		namespace ascii = boost::spirit::ascii;

		template<typename Char, typename Scheme>
		struct scheme_lexer;

		template<typename Scheme>
		struct scheme_lexer<char,Scheme> : qi::grammar<typename Scheme::iterator, typename Scheme::enum_t()> {
			typedef scheme_lexer<char,Scheme> self_type;

			scheme_lexer() : scheme_lexer::base_type(scheme_) {
				scheme_symbols_.add
					("http", Scheme::http)
					("https", Scheme::https)
					("ftp", Scheme::ftp)
				;
				scheme_ = scheme_symbols_;
			}

		private:
			qi::rule<typename Scheme::iterator, typename Scheme::enum_t()> scheme_;
			qi::symbols<char, typename Scheme::enum_t> scheme_symbols_;
		};
	}

	template<typename Char>
	struct basic_scheme {
		typedef basic_scheme<Char> self_type;

		// CharTraits
		typedef lulib::char_traits<Char>             char_traits;
		typedef typename char_traits::char_type      char_type;
		typedef typename char_traits::string_type    string_type;
		typedef typename string_type::const_iterator iterator;

		enum enum_t {
			undefined = -1,
			http,
			https,
			ftp,
		};

		static enum_t get_id(string_type const& scheme) {
			namespace ascii = boost::spirit::ascii;
			static scheme_detail::scheme_lexer<char_type, self_type> lexer;
			enum_t id = undefined;
			iterator begin = scheme.begin(), end = scheme.end();
			phrase_parse(begin, end, lexer, ascii::space, id);
			return id;
		}
	};

	typedef basic_scheme<char> scheme;

}}// namespace lulib::network
