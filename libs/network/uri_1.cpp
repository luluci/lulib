
#include <lulib/network/uri.hpp>

#include <boost/spirit/home/qi/operator/expect.hpp>

void func(std::string const& str) {
	typedef lulib::network::uri uri_type;
	uri_type uri(str);

	std::cout << "result    : " << std::boolalpha << static_cast<bool>(uri) << std::endl;
	std::cout << "uri       : " << uri.raw_uri() << std::endl;
	std::cout << "scheme    : " << uri.scheme() << std::endl;
	std::cout << "user_info : " << uri.user_info() << std::endl;
	std::cout << "host      : " << uri.host() << std::endl;
	std::cout << "port      : " << uri.port() << std::endl;
	std::cout << "port_str  : " << uri.port_str() << std::endl;
	std::cout << "file      : " << uri.file() << std::endl;
	std::cout << "query     : " << uri.query() << std::endl;
	std::cout << "fragment  : " << uri.fragment() << std::endl;
	std::cout << std::endl << std::endl;
}

int main() {
	typedef std::string::const_iterator Iterator;

	std::string str;
	while ( std::getline(std::cin, str) ) {
		try {
			func(str);
		} catch (boost::spirit::qi::expectation_failure<Iterator> &e) {
			std::cout << e.what() << std::endl;
			std::string str(e.first, e.last);
			std::cout << str << std::endl;
		}
	}
}

