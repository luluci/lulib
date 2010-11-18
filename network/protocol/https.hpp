#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <lulib/network/protocol/async/connect_ssl.hpp>
#include <lulib/network/protocol/async/write.hpp>
#include <lulib/network/protocol/async/read.hpp>
#include <lulib/network/protocol/parser/http_response.hpp>
#include <lulib/network/protocol/make_request/http.hpp>

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
			template<context_method ContextMethod = boost::asio::ssl::context::sslv23>
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

				typedef make_request::header_container header_container;

				// 通信メソッド定義
				struct method {
					struct GET {};
					struct POST {};
				};

			public:
				https(io_service &ios)
				: ios_(ios), context_(ios_, ContextMethod), socket_(ios_, context_), deadline_(ios_),
				stopped_(true)
				{
					verify(false);
					//
					deadline_.expires_at(boost::posix_time::pos_infin);
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

				socket_type& socket() { return socket_; }

				// port::host に接続する
				// port:http, host:www.hogehoge.com
				bool connect_async(std::string const& host, int timeout_sec, std::string const& service = "https") {
					// 処理開始
					stopped_ = false;
					// connect
					connect_async_impl(host, timeout_sec, service);

					return !stopped_;
				}
				// socketに書きこむ
				// GET method
				bool write_async(
					std::string const& host,
					std::string const& file,
					header_container const& headers,
					std::string const& option,
					int timeout_sec,
					typename method::GET
				) {
					if (stopped_) return false;
					// write
					write_async_impl( make_request::http_get(host, file, headers) , timeout_sec);

					return !stopped_;
				}
				// POST method
				bool write_async(
					std::string const& host,
					std::string const& file,
					header_container const& headers,
					std::string const& option,
					int timeout_sec,
					typename method::POST
				) {
					if (stopped_) return false;
					// write
					write_async_impl( make_request::http_post(host, file, headers, option) , timeout_sec);

					return !stopped_;
				}
				// socketから読み込む
				bool read_async(unsigned int &status_code, std::string &header, std::string &body, int timeout_sec) {
					if (stopped_) return false;
					// read
					read_async_impl(status_code, header, body, timeout_sec);

					bool result = !stopped_;

					// readまで終わったのでstop
					async_failure();

					return result;
				}

				// このプロトコルが開いているかどうか
				operator bool() {
					return !stopped_;
				}
				// async処理成功の通知
				void async_success() {
					// 処理が成功したのでdeadline_timerを止める
					deadline_.cancel();
				}
				// async処理失敗の通知
				void async_failure() {
					// 処理が中断される
					stopped_ = true;
					// その他リセット
					socket_.lowest_layer().close();
					deadline_.cancel();
				}
			private:
				void on_timeout(boost::system::error_code const& error) {
					// 手動でdeadline_timerがキャンセルされた
					if ( error == boost::asio::error::operation_aborted ) {
						// 手動でキャンセルするのは処理が終了した場合
						// なにもしない
					}
					// 手動キャンセル以外
					else {
						// エラーでないなら、タイムアウト
						if (!error) {
							error_message("TIMEOUT!");
						}
						// 何らかのエラー
						else {
							error_message(boost::system::system_error(error).what());
						}

						// エラーなので処理を中断する
						async_failure();
					}
				}

				// 設定したasync処理をスタートする
				void run() {
					// async処理の終了を待つ
					ios_.run();
					// 処理が終わったのでリセット
					ios_.reset();

				}

				// async処理: 処理結果が確定した段階(true or falseを指定して返すとき)でdeadlineを止める
				// port::host に接続する
				// port:http, host:www.hogehoge.com
				void connect_async_impl(std::string const& host, int timeout_sec, std::string const& service) {
					// 
					resolver_type resolver( ios_ );
					query_type query(host, service);
					endpoint_iterator ep_it = resolver.resolve(query);

					// deadlineをセット
					deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));

					//
					async::connect_ssl(*this, ep_it);

					// deadline_timerをセット
					deadline_.async_wait([&, this](boost::system::error_code const& error) {
						this->on_timeout(error);
					});

					// async処理の終了を待つ
					run();

					return;
				}

				void write_async_impl(std::string && mess, int timeout_sec) {
					// リクエスト作成
					boost::asio::streambuf request;
					std::ostream request_stream( &request );
					request_stream << std::move(mess);

					// deadlineをセット
					deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));

					async::write(*this, request);

					// deadline_timerをセット
					deadline_.async_wait([&, this](boost::system::error_code const& error) {
						// timeoutによる終了(false)のみresultに適用する
						this->on_timeout(error);
					});

					// 設定したasync処理をスタートする
					run();

					return;
				}

				void read_async_impl(
					unsigned int &status_code,
					std::string &header,
					std::string &body,
					int timeout_sec
				) {
					// deadlineをセット
					deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));

					boost::asio::streambuf response;
					async::read(*this, response);

					// deadline_timerをセット
					deadline_.async_wait([&, this](boost::system::error_code const& error) {
						// timeoutによる終了(false)のみresultに適用する
						this->on_timeout(error);
					});

					// 設定したasync処理をスタートする
					run();

					parser::http_response(response, status_code, header, body);

					return;
				}

			private:
				io_service &ios_;
				ssl_context context_;
				socket_type socket_;
				deadline_timer deadline_;
				// timeoutその他により処理が終了している
				bool stopped_;

				// 
				boost::asio::streambuf response_;
			};

		}//namespace protocol
	}//namespace network
}//namespace lulib
