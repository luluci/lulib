#pragma once
#pragma warning(disable : 4819)

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/container.hpp>

#include <boost/optional.hpp>

#include <lulib/network/uri/detail/uri_parts.hpp>
#include <lulib/type_traits/char_traits.hpp>

namespace lulib { namespace network { namespace uri_detail {

	namespace qi = boost::spirit::qi;
	namespace phx = boost::phoenix;
	namespace ascii = boost::spirit::ascii;

	template<typename Iterator>
	struct uri_parser : qi::grammar<Iterator> {

		// CharTraits
		typedef lulib::char_traits<char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		uri_parser(uri_parts &parts) : uri_parser::base_type(uri_) {
			char const* digit = "0-9";
			char const* alpha = "a-zA-Z";
			//char const* alnum = "a-zA-Z0-9";

			// パーセントエンコーディング pct-encoded
			char const* hex = "a-fA-F0-9";
			// 予約文字
			//char const* gen_delims = ":/?#[]@";
			//char const* sub_delims = "!$&'()*+,;=";
			//char const* reserved   = ":/?#[]@!$&'()*+,;=";
			// 非予約文字
			//char const* unreserved = "a-zA-Z0-9._~-";

			char const* query_c = "?/:@!$&'()*+,;=a-zA-Z0-9._~-";
			char const* pchar_c = ":@!$&'()*+,;=a-zA-Z0-9._~-";
			char const* segment_nz_nc_c = "@!$&'()*+,;=a-zA-Z0-9._~-";
			char const* reg_name_c = "!$&'()*+,;=a-zA-Z0-9._~-";        // unreserved + sub-delims
			char const* ipvfuture_c = ":!$&'()*+,;=a-zA-Z0-9._~-";
			char const* userinfo_c = ":!$&'()*+,;=a-zA-Z0-9._~-";
			char const* scheme_c = "a-zA-Z0-9+.-";

			// pct-encoded = '%' > hex > hex
			pct_encoded_ %= qi::char_('%') >> qi::char_(hex) >> qi::char_(hex);

			// 3.5. Fragment
			fragment_ = qi::as_string[ *qi::char_(query_c) ][ phx::ref(parts.fragment) = qi::_1 ];  // queryと同じ

			// 3.4. Query
			query_ = qi::as_string[ *qi::char_(query_c) ][ phx::ref(parts.query) = qi::_1 ];

			// 3.3. Path
			pchar_         %= (qi::char_(pchar_c) | pct_encoded_);
			pchar_nz_nc_   %= (qi::char_(segment_nz_nc_c) | pct_encoded_);
			segment_nz_nc_ %= +pchar_nz_nc_;
			segment_nz_    %= +pchar_;
			segment_       %= *pchar_;
			path_empty_    %= qi::char_('\0');
			path_rootless_ %= segment_nz_ >> *(qi::char_('/') >> segment_);
			path_noscheme_ %= segment_nz_nc_ >> *(qi::char_('/') >> segment_);
			path_absolute_ %= qi::char_('/') >> -(segment_nz_ >> *(qi::char_('/') >> segment_));
			path_abempty_  %= *(qi::char_('/') >> segment_);
			path_ %= (
				path_abempty_     // "/"で始まるか、空
				| path_absolute_  // "/"で始まるが、"//"では始まらない
				| path_noscheme_  // コロンのない segment で始まる
				| path_rootless_  // segment で始まる
				| path_empty_     // 文字がない
			);

			// 3.2.3. Port
			port_ = qi::as_string[ *qi::char_(digit) ][ phx::ref(parts.port) = qi::_1 ];
			//port_ = qi::int_[ phx::ref(parts.port) = qi::_1 ];

			// 3.2.2. Host
			// reg-name
			reg_name_ %= *( qi::char_(reg_name_c) | pct_encoded_ );
			// IPv4address
			// dec-octet = DIGIT              ; 0-9
			//           / %x31-39 DIGIT      ; 10-99
			//           / "1" 2DIGIT         ; 100-199
			//           / "2" %x30-34 DIGIT  ; 200-249
			//           / "25" %x30-35       ; 250-255
			dec_octet_ %= qi::raw[ qi::uint_parser<char, 10, 0, 255>() ];
			ipv4address_ %= qi::as_string[
				dec_octet_
				>> qi::char_('.') >> dec_octet_
				>> qi::char_('.') >> dec_octet_
				>> qi::char_('.') >> dec_octet_
			];
			// IP-literal
			h16_ = qi::repeat(1,4)[ qi::char_(hex) ];
			ls32_ = ((h16_ >> qi::char_(':') >> h16_) | ipv4address_);
			ipv6address_ = (
				(qi::repeat(6)[ h16_ >> qi::char_(':') ] >> ls32_)
				| (qi::string("::") >> qi::repeat(5)[ h16_ >> qi::char_(':') ] >> ls32_)
				| (-(h16_) >> qi::string("::") >> qi::repeat(4)[ h16_ >> qi::char_(':') ] >> ls32_)
				| (-(qi::repeat(0,1)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::") >> qi::repeat(3)[ h16_ >> qi::char_(':') ] >> ls32_)
				| (-(qi::repeat(0,2)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::") >> qi::repeat(2)[ h16_ >> qi::char_(':') ] >> ls32_)
				| (-(qi::repeat(0,3)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::") >> h16_ >> qi::char_(':') >> ls32_)
				| (-(qi::repeat(0,4)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::") >> ls32_)
				| (-(qi::repeat(0,5)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::") >> h16_)
				| (-(qi::repeat(0,6)[ h16_ >> qi::char_(':') ] >> h16_) >> qi::string("::"))
			);
			ipvfuture_ %= qi::char_('v') >> +qi::char_(hex) >> qi::char_('.') >> +qi::char_(ipvfuture_c);
			ip_literal_ %= qi::char_('[') >> (ipv6address_ | ipvfuture_) >> qi::char_(']');
			host_ = (ip_literal_ | ipv4address_ | reg_name_)[ phx::ref(parts.host) = qi::_1 ];

			// 3.2.1. user information
			userinfo_ = qi::as_string[
				*(qi::char_(userinfo_c) | pct_encoded_)
			];

			// 3.2. authority
			authority_ %= -(
				qi::hold[qi::as_string[userinfo_ >> '@']][ phx::ref(parts.user_info) = qi::_1 ]
			) >> host_ >> -(':' >> port_);

			// 3.1. スキーム
			scheme_ = qi::as_string[
				qi::char_(alpha) >> *qi::char_(scheme_c)
			][ phx::ref(parts.scheme) = qi::_1 ];

			// 3. 構文の構成要素
			hier_part_ = (
				(qi::string("//") >> authority_ >> path_abempty_[ phx::ref(parts.file) = qi::_1 ])
				| path_absolute_[ phx::ref(parts.file) = qi::_1 ]
				| path_rootless_[ phx::ref(parts.file) = qi::_1 ]
				| path_empty_[ phx::ref(parts.file) = qi::_1 ]
			);

			uri_ = scheme_ > qi::char_(':') > hier_part_ >> -(qi::char_('?') > query_) >> -(qi::char_('#') >> fragment_);
		}

		typedef qi::rule<Iterator> base_rule;
		typedef qi::rule<Iterator, string_type()> str_rule;

		// 3.
		base_rule uri_;
		str_rule hier_part_;
		str_rule query_;
		str_rule fragment_;
		// 3.1. Scheme
		str_rule scheme_;
		// 3.2. Authority
		str_rule authority_;
		str_rule userinfo_;
		str_rule userinfo_char_, userinfo_str_;
		str_rule host_;
		str_rule port_;
		// 3.2.2.
		str_rule ip_literal_, ipv4address_;
		str_rule reg_name_;
		str_rule dec_octet_;
		str_rule ipv6address_, ipvfuture_;
		str_rule ls32_, h16_;
		// 3.3. Path
		str_rule path_;
		str_rule path_abempty_, path_absolute_, path_noscheme_, path_rootless_, path_empty_;
		str_rule segment_, segment_nz_, segment_nz_nc_;
		str_rule pchar_, pchar_nz_nc_;
		// percent encoded
		str_rule pct_encoded_;

	};

	boost::optional<uri_parts> parse_uri(std::string const& str) {
		typedef std::string::const_iterator Iterator;

		uri_parts uri;
		uri_parser<Iterator> p(uri);

		Iterator begin = str.begin(), end = str.end();
		if ( phrase_parse(begin, end, p, ascii::space) ) {
			return boost::optional<uri_parts>(uri);
		}
		else {
			return boost::optional<uri_parts>();
		}
	}

}}}// namespace lulib::network::uri_detail
