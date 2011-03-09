#pragma once

#include <string>
#include <map>
#include <boost/asio/io_service.hpp>

namespace lulib {
	namespace network {

		template<typename Protocol>
		class basic_client {
			typedef boost::asio::io_service io_service;

			// トランスポート層のプロトコル
			typedef Protocol protocol_type;

		public:
			typedef typename protocol_type::header_container header_container;
			// プロトコルに属する通信メソッド
			typedef typename protocol_type::method method;

		public:
			basic_client()
			: ios_(), protocol_(ios_)
			{
			}

			// プロトコルに対する直接操作をしたいな
			inline protocol_type& protocol() { return protocol_; }

			// async
			template<typename Method>
			bool operator()(
				// 引数
				std::string      const& host,         // host(domain)
				std::string      const& file,         // file
				header_container const& http_header,  // http header
				std::string      const& option,       // option data
				// 返り値
				unsigned int& status_code,  // response status code
				std::string& header,        // response header
				std::string& body,          // response body
				// タイムアウトまでの時間
				int timeout,
				// メソッド
				Method
			) {
				/*
				if ( !protocol_.connect_async(host, timeout) ) return false;
				if ( !protocol_.write_async(host, file, http_header, option, timeout, Method()) ) return false;
				if ( !protocol_.read_async(status_code, header, body, timeout) ) return false;
				return true;
				*/
				return protocol_(host, file, http_header, option, status_code, header, body, timeout, Method());
			}

		private:
			io_service ios_;
			protocol_type protocol_;
		};

	}//namespace network
}// namespace lulib

