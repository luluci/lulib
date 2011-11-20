
#include <iostream>

#include <lulib/network/protocol/http.hpp>

namespace http = lulib::network::http;

void print(http::client::response &resp) {
	auto it = resp.header().begin();
	auto end = resp.header().end();
	std::cout << "response header:" << std::endl;
	while (it != end) {
		std::cout << it->first << " : " << it->second << std::endl;
		++it;
	}
	std::cout << "response body:" << std::endl;
	std::cout << resp.body() << std::endl;
}

int main() {

	http::client client(120);
	typedef http::client::response response;

	std::string url;
	while (std::getline(std::cin, url)) {
		http::request request(url);
		response res = client << GET(request);
		print(res);
	}

}
