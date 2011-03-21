#pragma once

#include <string>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

#include <lulib/network/protocol/async/connect.hpp>
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

namespace lulib { namespace network { namespace protocol {
	// http通信プロトコル型
	class http {
		// io_service
		typedef boost::asio::io_service io_service;
		// socket type
		typedef boost::asio::ip::tcp::socket tcp_socket;

		// deadline_timer
		typedef boost::asio::deadline_timer deadline_timer;

	public:
		// common type
		typedef tcp_socket socket_type;
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
		http(io_service &ios)
		: ios_(ios), socket_(ios_), deadline_(ios_), stopped_(true)
		{
			//
			deadline_.expires_at(boost::posix_time::pos_infin);
		}

		socket_type& socket() { return socket_; }
		
		// async
		// GET
		inline bool operator()(
			// 引数
			std::string      const& host,         // host(domain)
			std::string      const& file,         // file
			header_container const& headers,  // http header
			std::string      const& option,       // option data
			// 返り値
			unsigned int& status_code,  // response status code
			std::string& header,        // response header
			std::string& body,          // response body
			// タイムアウトまでの時間
			int timeout,
			// メソッド
			method::GET
		) {
			// フラグ初期化
			stopped_ = false;
			// ステータスコードをリセット
			status_code = 0;
			// request作成
			boost::asio::streambuf request;
			make_request::http_get(request, host, file, headers);
			// 処理開始
			run_impl(host, "http", request, status_code, header, body, timeout);
			// readまで終わったのでstop
			async_failure();

			// ステータスコードで正否判断
			bool result = (status_code == 200);
			return result;
		}
		// POST
		inline bool operator()(
			// 引数
			std::string      const& host,         // host(domain)
			std::string      const& file,         // file
			header_container const& headers,  // http header
			std::string      const& option,       // option data
			// 返り値
			unsigned int& status_code,  // response status code
			std::string& header,        // response header
			std::string& body,          // response body
			// タイムアウトまでの時間
			int timeout,
			// メソッド
			method::POST
		) {
			// フラグ初期化
			stopped_ = false;
			// ステータスコードをリセット
			status_code = 0;
			// request作成
			boost::asio::streambuf request;
			make_request::http_post(request, host, file, headers, option);
			// 処理開始
			run_impl(host, "http", request, status_code, header, body, timeout);
			// readまで終わったのでstop
			async_failure();

			// ステータスコードで正否判断
			bool result = (status_code == 200);
			return result;
		}

		// port::host に接続する
		// port:http, host:www.hogehoge.com
		bool connect_async(std::string const& host, int timeout_sec, std::string const& service = "http") {
			stopped_ = false;  // 処理開始
			connect_async_impl(host, timeout_sec, service);  // connect
			return !stopped_;  // stopしていたらtimeout
		}
		// socketに書きこむ
		// GET method
		bool write_async(
			std::string const& host,
			std::string const& file,
			header_container const& headers,
			std::string const& option,
			int timeout_sec,
			method::GET
		) {
			if (stopped_) return false;
			boost::asio::streambuf request;
			make_request::http_get(request, host, file, headers);
			write_async_impl(request, timeout_sec);  // write
			return !stopped_;  // stopしていたらtimeout
		}
		// POST method
		bool write_async(
			std::string const& host,
			std::string const& file,
			header_container const& headers,
			std::string const& option,
			int timeout_sec,
			method::POST
		) {
			if (stopped_) return false;
			boost::asio::streambuf request;
			make_request::http_post(request, host, file, headers, option);
			write_async_impl(request, timeout_sec);  // write
			return !stopped_;  // stopしていたらtimeout
		}
		// socketから読み込む
		bool read_async(unsigned int &status_code, std::string &header, std::string &body, int timeout_sec) {
			if (stopped_) return false;
			status_code = 0;  // ステータスコードをリセット
			read_async_impl(status_code, header, body, timeout_sec);  // read
			async_failure();          // readまで終わったのでstop

			// ステータスコードで正否判断
			return (status_code == 200);
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
			socket_.close();
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

		void run_impl(
			std::string const& host,
			std::string const& service,
			boost::asio::streambuf &request,
			unsigned int &status_code,
			std::string &header,
			std::string &body,
			int timeout_sec
		) {
			// 変数を初期化
			resolver_type resolver( ios_ );
			query_type query(host, service);
			// resolve() は例外投げるよ！
			//endpoint_iterator ep_it = resolver.resolve(query);
			///*
			endpoint_iterator ep_it;
			try {
				ep_it = resolver.resolve(query);
			}
			catch (boost::system::system_error) {
				async_failure();
				return;
			}
			//*/
			boost::asio::streambuf response;

			// コールバックファンクタを作成
			auto success = [this](){ this->async_success(); };
			auto failure = [this](){ this->async_failure(); };
			auto start_read = [&, this](){
				async::read(*this, response, success);  // read
			};
			auto start_write = [&, this](){
				async::write(*this, request, start_read, failure);  // write
			};
			// 通信をセット
			async::connect(*this, ep_it, start_write, failure);  // connect

			// deadlineをセット
			deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));
			// deadline_timerをセット
			deadline_.async_wait([this](boost::system::error_code const& error) {
				// timeoutによる終了(false)のみresultに適用する
				this->on_timeout(error);
			});
			// 設定したasync処理をスタートする
			run();
			// レスポンスを解析
			parser::http_response(response, status_code, header, body);

			return;
		}

		void connect_async_impl(std::string const& host, int timeout_sec, std::string const& service) {
			// 変数を初期化
			resolver_type resolver( ios_ );
			query_type query(host, service);
			// resolve() は例外投げるよ！
			//endpoint_iterator ep_it = resolver.resolve(query);
			///*
			endpoint_iterator ep_it;
			try {
				ep_it = resolver.resolve(query);
			}
			catch (boost::system::system_error) {
				async_failure();
				return;
			}
			//*/
			// deadlineをセット
			deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));
			// 通信をセット
			async::connect(*this, ep_it,
				[this](){ this->async_success(); },
				[this](){ this->async_failure(); }
			);
			// deadline_timerをセット
			deadline_.async_wait([this](boost::system::error_code const& error) {
				// timeoutによる終了(false)のみresultに適用する
				this->on_timeout(error);
			});
			// 設定したasync処理をスタートする
			run();

			return;
		}

		void write_async_impl(boost::asio::streambuf &request, int timeout_sec) {
			// deadlineをセット
			deadline_.expires_from_now(boost::posix_time::seconds(timeout_sec));
			// write
			async::write(*this, request,
				[this](){ this->async_success(); },
				[this](){ this->async_failure(); }
			);
			// deadline_timerをセット
			deadline_.async_wait([this](boost::system::error_code const& error) {
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
			// read
			boost::asio::streambuf response;
			async::read(*this, response,
				[this](){ this->async_success(); }
			);
			// deadline_timerをセット
			deadline_.async_wait([this](boost::system::error_code const& error) {
				// timeoutによる終了(false)のみresultに適用する
				this->on_timeout(error);
			});
			// 設定したasync処理をスタートする
			run();
			// レスポンスを解析
			parser::http_response(response, status_code, header, body);

			return;
		}

	private:
		io_service &ios_;
		socket_type socket_;
		deadline_timer deadline_;
		// timeoutその他により処理が終了している
		bool stopped_;
	};

}}}//namespace lulib::network::protocol
