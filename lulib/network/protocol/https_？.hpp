#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <lulib/network/detail/write.hpp>

#ifdef DEBUG
#include <iostream>
#define error_message(t) std::cerr << t << std::endl;
#else
#define error_message(t)
#endif

namespace lulib {
	namespace network {
		namespace protocol {
			// enum型
			typedef boost::asio::ssl::context::method context_method;

			// https通信プロトコル型
			template<context_method Method = boost::asio::ssl::context::sslv23>
			class https {
				// io_service
				typedef boost::asio::io_service io_service;
				// socket type
				typedef boost::asio::ip::tcp::socket tcp_socket;
				typedef boost::asio::ssl::stream<tcp_socket> tcp_ssl_socket;
				// SSL context type
				typedef boost::asio::ssl::context ssl_context;

				// deadline_timer
				typedef boost::asio::deadline_timer deadline_timer;

			public:
				// common type
				typedef tcp_ssl_socket socket_type;
				typedef boost::asio::ip::tcp::endpoint endpoint_type;
				typedef boost::asio::ip::tcp::resolver resolver_type;
				typedef boost::asio::ip::tcp::resolver::query query_type;
				typedef boost::asio::ip::tcp::resolver::iterator endpoint_iterator;

			private:
				// 
				//typedef std::unique_ptr<socket_type> socket_ptr;

			public:
				https(io_service &ios)
				: ios_(ios), context_(ios_, Method), socket_(ios_, context_), deadline_(ios_),
				stopped_(true)
				{
					//
					deadline_.expires_at(boost::posix_time::pos_infin);

					check_deadline();
				}

				void verify(bool v, std::string const& cac_path = "") {
					// 認証をするなら
					if (v) {
						// モードをセット
						context_.set_verify_mode(boost::asio::ssl::context::verify_peer);
						// CA証明書までのパスをセット
						context_.load_verify_file(cac_path);
					}
					else {
						context_.set_verify_mode(boost::asio::ssl::context::verify_none);
					}
				}

				// port::host に接続する
				// port:http, host:www.hogehoge.com
				bool connect_async(std::string const& host, int timeout_sec, std::string const& service = "https") {
					// connect
					bool result = connect_async_impl(host, timeout_sec, service);
					return result;
				}
				// socketに書きこむ
				bool write_async() {
					return true;
				}

			private:
				void check_deadline() {
					// 
					if (deadline_.expires_at() <= deadline_timer::traits_type::now()) {
						socket_.lowest_layer().close();
						deadline_.expires_at(boost::posix_time::pos_infin);
					}

					// Put the actor back to sleep.
					deadline_.async_wait([=](boost::system::error_code const& error){
						this->check_deadline();
					});
				}

				// port::host に接続する
				// port:http, host:www.hogehoge.com
				bool connect_async_impl(std::string const& host, int timeout_sec, std::string const& service) {
					// Resolve the host name and service to a list of endpoints.
					resolver_type resolver(ios_);
					query_type query(host, service);
					endpoint_iterator iter = resolver.resolve(query);

					// Set a deadline
					deadline_.expires_from_now(boost::posix_time::milliseconds(timeout_sec));

					// 
					boost::system::error_code ec;
					for (; iter != endpoint_iterator(); ++iter)
					{
						// 無効なsocketを閉じる
						socket_.lowest_layer().close();

						// エラーのデフォルト値をセット
						ec = boost::asio::error::would_block;
						// async connect開始
						socket_.lowest_layer().async_connect(iter->endpoint(),
							[&](boost::system::error_code const& error) {
								ec = error;  // コールバックされるエラーコードを通知
							}
						);

						// async処理が終了するまでブロック
						do {
							ios_.run_one(); 
						} while (ec == boost::asio::error::would_block);

						// connectに成功した: trueを返す
						if (!ec && socket_.lowest_layer().is_open()) return true;
					}

					// connectに失敗
					return false;
				}
				bool handshake() {
					// すでに処理が中断されていたら終了
					if (stopped_) return false;

					bool result;
					socket_.async_handshake( boost::asio::ssl::stream_base::client,
						[&, this](boost::system::error_code const& error) {
							if (!error) result = true;
							else result = false;
						}
					);

					// handshakeに失敗
					if (!result) {
						// 処理を中断する
						stop();
					}

					// handshakeの結果を返す
					return result;
				}

			private:
				io_service &ios_;
				ssl_context context_;
				socket_type socket_;
				deadline_timer deadline_;
				// timeoutその他により処理が終了している
				bool stopped_;
			};

		}//namespace protocol
	}//namespace network
}//namespace lulib
