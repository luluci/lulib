#pragma once
#pragma warning(disable : 4819)

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string/find.hpp>

#include <lulib/lexical_cast.hpp>
#include <lulib/type_traits/char_traits.hpp>

/*
#define BOOST_SPIRIT_USE_PHOENIX_V3 1
#include <boost/phoenix.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>

#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
//#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/qi_eol.hpp>
#include <boost/spirit/include/qi_kleene.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_plus.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_difference.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
#include <boost/spirit/include/qi_as.hpp>
#include <boost/spirit/include/support_argument.hpp>
#undef  BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
*/

namespace lulib { namespace network { namespace http { namespace detail {

	namespace spirit = boost::spirit;
	namespace qi = boost::spirit::qi;
	namespace phx = boost::phoenix;

	template<typename Container, typename Iterator>
	struct status_line_parser : qi::grammar<Iterator, void(), qi::blank_type> {
		typedef typename Iterator::value_type     char_type;
		typedef lulib::char_traits<char_type>     char_traits;
		typedef typename char_traits::string_type string_type;

		status_line_parser(Container &c) : status_line_parser::base_type(rule_) {
			qi::lit_type             lit;
			qi::char_type          char_;
			qi::eol_type             eol;
			qi::no_skip_type     no_skip;
			qi::as_string_type as_string;
			spirit::_1_type           _1;

			version_ %= no_skip[+char_("0-9") >> char_('.') >> +char_("0-9")];
			status_code_ %= qi::uint_type();
			status_      %= no_skip[+(char_ - '\r')];
			rule_ =
				lit("HTTP/")
				>> version_    [ phx::bind(&Container::version,     c, _1) ]
				>> status_code_[ phx::bind(&Container::status_code, c, _1) ]
				>> status_     [ phx::bind(&Container::status,      c, _1) ]
				>> eol
			;
		}

	private:
		typedef qi::rule<Iterator,        void(), qi::blank_type> rule;
		typedef qi::rule<Iterator, string_type(), qi::blank_type> str_rule;
		typedef qi::rule<Iterator, std::size_t(), qi::blank_type> uint_rule;

		rule      rule_;
		str_rule  version_;
		uint_rule status_code_;
		str_rule  status_;
	};

	template<typename Container, typename Iterator>
	struct header_parser : qi::grammar<Iterator, void(), qi::blank_type> {
		typedef typename Iterator::value_type     char_type;
		typedef lulib::char_traits<char_type>     char_traits;
		typedef typename char_traits::string_type string_type;

		header_parser(Container &c) : header_parser::base_type(rule_) {
			qi::char_type          char_;
			qi::eol_type             eol;
			qi::as_string_type as_string;
			spirit::_1_type           _1;
			spirit::_2_type           _2;

			http_header_ = (
				as_string[+(char_ - char_(":\r\n"))]
				>> ':'
				>> as_string[+(char_ - char_("\r\n"))]
				>> eol
			)[ phx::bind(&Container::header, c, _1, _2) ];
			rule_ = *http_header_ >> eol;
		}

	private:
		typedef qi::rule<Iterator, void(), qi::blank_type> rule;

		rule      rule_;
		rule      http_header_;
	};


	// parse http response, header
	template<typename Container, typename Iterator>
	bool parse_response_header(Container &c, Iterator &it, Iterator end) {
		typedef boost::iterator_range<Iterator> irange;
		irange rng = boost::make_iterator_range(it, end);
		irange find_result = boost::find_first(rng, "\r\n\r\n");  // headerの区切りを検索
		if (!find_result) {    // 区切りが見つからなかった
			return false;  // 次のレスポンス取得を待つ
		}

		// rule
		status_line_parser<Container, Iterator> status_line(c);
		header_parser<Container, Iterator>      header(c);
		// parse start
		bool result = phrase_parse(it, end, status_line >> header, qi::blank_type());
		return result;
	}

	// parse http response, body - default




	// parse http response, body - chunked
	// chunked body の解析
	//   container : responseコンテナ
	//   it, end   : buffer iterator
	//   chunk_size: 残りchunkサイズ
	template<typename Container, typename Iterator>
	bool parse_response_body_chunked(Container &c, Iterator &it, Iterator end) {
		// chunk取得残りサイズがあるなら
		switch (c.state_) {
			case Container::state::body_chunk: {
				return parse_response_body_chunked_chunk(c, it, end);
			}
			case Container::state::body_chunk_trailer: {
				return parse_response_body_chunked_trailer(c, it, end);
			}
		}
		return false;
	}

	// Chunked-Body: chunk
	template<typename Container, typename Iterator>
	bool parse_response_body_chunked_chunk(Container &c, Iterator &it, Iterator end) {
		if (c.chunk_rest_ > 0) {
			// chunk取得
			return parse_response_body_chunked_chunk_data(c, it, end);
		}
		// chunked-sizeからの取得なら
		else {
			return parse_response_body_chunked_chunk_size(c, it, end);
		}
		return false;
	}
	template<typename Container, typename Iterator>
	bool parse_response_body_chunked_chunk_size(Container &c, Iterator &it, Iterator end) {
		auto range = boost::make_iterator_range(it, end);  // input range
		// rangeから\r\nを検索
		auto result = boost::algorithm::find_first(range, "\r\n");
		// chunkまで読み込み済みなら
		if (result) {
			/*
			{
				std::cout << "chunk size:" << std::endl;
				Iterator i = it;
				for (int j=0; i != end && j < 5; ++i, ++j) {
					std::cout << *i;
				}
				std::cout << std::endl;
			}
			*/
			if ( auto chunk_size = lulib::lexical_cast<lulib::as_hex32_opt>(it, result.begin()) ) {
				c.chunk_rest_ = *chunk_size;
				it = result.end();
				// last-chunk(0 [ chunk-extension ] CRLF)が現れたら
				if (c.chunk_rest_ == 0) {
					c.state_ = Container::state::body_chunk_trailer;  // chunk-trailer解析状態に
					return parse_response_body_chunked(c, it, end);   // バッファ残りを解析
				}
				// バッファの残りを解析
				return parse_response_body_chunked_chunk(c, it, end);
			}
		}
		return false;
	}
	template<typename Container, typename Iterator>
	bool parse_response_body_chunked_chunk_data(Container &c, Iterator &it, Iterator end) {
		std::size_t diff = end - it;  // bufferサイズを計算

		// bufferが全部chunkに収まるなら
		if (diff <= c.chunk_rest_) {
			c.body_.append(it, end);
			it = end;
			c.chunk_rest_ -= diff;
			return true;
		}
		// chunkが途中までなら
		else {
			// chunk部分を計算して追加
			Iterator chunk_end = it + c.chunk_rest_;
			c.body_.append(it, chunk_end);
			// 次のchunk
			it = chunk_end + 2;       // \r\nの分を加算
			c.chunk_rest_ = 0;  // chunk sizeリセット
			return parse_response_body_chunked_chunk(c, it, end);  // chunkサイズの取得から再開
		}
		return false;
	}

	// Chunked-Body: trailer
	template<typename Container, typename Iterator>
	bool parse_response_body_chunked_trailer(Container &c, Iterator &it, Iterator end) {
		/*
		std::cout << "rest at trailer start:" << std::endl;
		{
			Iterator i = it;
			while (i != end) {
				std::cout << *i; ++i;
			}
			std::cout << std::endl;
		}
		*/

		// rule
		header_parser<Container, Iterator> header(c);
		// parse start
		bool parse_result = phrase_parse(it, end, header, qi::blank_type());
		// trailerまで終了
		if (!parse_result) return false;

		// 読み込み終了
		c.state_ = Container::state::eof;
		return true;
	}

}}}}

