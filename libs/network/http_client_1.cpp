
#include <iostream>

#include <lulib/network/protocol/http.hpp>

namespace http = lulib::network::http;

void handler(http::client::response &resp) {
	std::cerr << "response size:" << resp.body().size() << std::endl;
	std::cout << "response body:" << std::endl;
	std::cout << resp.body() << std::endl;
}

int main() {

	http::client client(120);
	typedef http::client::response response;
	//client.set_callback(handler);
	{
		std::cerr << "request : socket is open? " << client.is_open() << std::endl;
		//http::request request("http://www5.atpages.jp/luluci/");
		http::request request("http://luluci.blog44.fc2.com/");
		//request << http::header("Connection", "Keep-Alive");
		//request << http::header("Connection", "close");
		request << http::header("Accept", "*/*");
		response res;
		client << GET(request);
		res = client;
		handler(res);
		//client << GET(request);
	}
	{
		std::cerr << "request2: socket is open? " << client.is_open() << std::endl;
		http::request request("http://luluci.blog44.fc2.com/page-1.html");
		request << http::header("Connection", "close");
		request << http::header("Accept", "*/*");
		response res = client << GET(request);
		handler(res);
	}
	//client << GET(request);
	//client.get(request);

	//std::string finish;
	//std::cin >> finish;

	/*
	namespace http = lulib::network::http;
	http::request request("http://twitter.com/status_.json");
	request << http::header("name1", "value1");
	request << http::header("name2", "value2");
	request << http::header("name1", "value3");
	request << http::query("q1", "data1");
	request << http::query("q2", "data2");
	request << http::query("q1", "data5");
	*/

	/*
	std::cout << request.scheme_ << std::endl;
	std::cout << request.scheme_id_ << std::endl;
	std::cout << request.host_ << std::endl;
	std::cout << request.port_ << std::endl;
	auto it = request.headers_.begin(), end = request.headers_.end();
	while (it != end) {
		std::cout << it->first << ":" << it->second << std::endl;
		++it;
	}
	*/

	/*
	{
		boost::asio::streambuf buf;
		typedef http::GET<1,1,char> GET;
		GET::make_request(request, buf);
		std::istream ist(&buf);
		std::string str;
		std::cout << "buff size: " << buf.size() << std::endl;
		while ( std::getline(ist, str) ) {
			std::cout << str << std::endl;
		}
		std::cout << "buff size: " << buf.size() << std::endl << std::endl;
	}
	{
		boost::asio::streambuf buf;
		typedef http::POST<1,1,char> POST;
		POST::make_request(request, buf);
		std::istream ist(&buf);
		std::string str;
		std::cout << "buff size: " << buf.size() << std::endl;
		while ( std::getline(ist, str) ) {
			std::cout << str << std::endl;
		}
		std::cout << "buff size: " << buf.size() << std::endl << std::endl;
	}
	*/

	
}
