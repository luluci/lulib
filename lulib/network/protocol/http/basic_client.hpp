#pragma once
#pragma warning(disable : 4819)

#include <memory>
#include <functional>

#include <boost/thread.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#include <lulib/type_traits/char_traits.hpp>
#include <lulib/detail/debug_output.hpp>

#include <lulib/network/scheme.hpp>
#include <lulib/network/detail/connect.hpp>
#include <lulib/network/detail/connect_ssl.hpp>
#include <lulib/network/detail/write.hpp>
#include <lulib/network/detail/read.hpp>
#include <lulib/network/protocol/http/basic_request.hpp>
#include <lulib/network/protocol/http/basic_response.hpp>
#include <lulib/network/traits/transport.hpp>
#include <lulib/network/traits/ssl.hpp>

namespace lulib { namespace network { namespace http {

	// http通信プロトコル型
	template<typename Transport, typename Char>
	class basic_client {
		typedef basic_client<Transport, Char> self_type;

	public:
		// CharTraits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;

		typedef boost::asio::io_service      io_service;      // io_service
		typedef io_service::work             work;            // work
		typedef boost::asio::deadline_timer  deadline_timer;  // deadline_timer
		typedef std::size_t                  timer_type;
		typedef boost::system::error_code    error_code;

		typedef basic_scheme<Char> scheme;

		// トランスポート層プロトコル
		typedef Transport tp_tag;
		typedef transport<Transport>                transport_traits;
		typedef typename transport_traits::socket   socket_type;
		typedef typename transport_traits::endpoint endpoint;
		typedef typename transport_traits::resolver resolver;
		typedef typename resolver::protocol_type    ip_value;  // ip type
		typedef typename resolver::query            query;
		typedef typename resolver::iterator         endpoint_iterator;
		// SSL
		typedef ssl<Transport> ssl_traits;
		typedef typename ssl_traits::context context;
		typedef typename ssl_traits::socket  ssl_socket_type;

		// request
		typedef basic_request<Char> request;
		// response
		typedef basic_response<Char> response;

		// マルチスレッドで通信する場合、
		// コールバックファンクタを設定する
		typedef std::function<void(request&)> callback_functor;

	public:
		// io_serviceが渡されなかった場合、自分が作成
		basic_client(timer_type timeout = boost::posix_time::pos_infin)
		: ios_ptr_(new io_service()), ios_(*ios_ptr_), resolver_(ios_)
		, socket_(ios_), context_(context::sslv23), ssl_socket_(ios_, context_)
		, timeout_(timeout), deadline_(ios_), scheme_id_(scheme::undefined)
		{
			init_ssl();
		}
		// io_serviceが渡された場合
		basic_client(io_service &ios, timer_type timeout = boost::posix_time::pos_infin)
		: ios_(ios), resolver_(ios_)
		, socket_(ios_), context_(context::sslv23), ssl_socket_(ios_, context_)
		, timeout_(timeout), deadline_(ios_), scheme_id_(scheme::undefined)
		{
			init_ssl();
		}

		// dtor
		~basic_client() {
			work_.reset();
			if (thread_.joinable()) thread_.join();
		}

	public:
		// io_serviceへの参照を返す
		inline io_service& get_io_service() {
			return ios_;
		}

		// TimeOut値の設定
		inline void timeout(std::size_t time) {
			timeout_ = time;
		}

		// request取得時にコールバック
		template<typename Callback>
		bool set_callback(Callback &&callback) {
			// io_service::run()を別スレッドで行い、完全非同期化
			// 通信の結果はコールバックファンクタからのみ返される
			work_.reset( new work(ios_) );
			if (!work_) return false;
			thread_ = boost::thread( boost::bind(&io_service::run, &ios_) );
			if (!thread_.joinable()) {
				work_.reset();
				return false;
			}
			// response handler
			callback_ = std::forward<Callback>(callback);
			if (!callback_) {
				work_.reset();
				thread_.join();
				return false;
			}
			return true;
		}
		void reset_callback() {
			// 非同期化を解除
			// レスポンスはoperator=から行う
			work_.reset();
		}
		bool is_async() {
			return (work_ && callback_);
		}

		// このプロトコルが開いているかどうか
		bool is_open() {
			switch (scheme_id_) {
				case request::scheme::http: {
					return socket_.is_open();
				}
				case request::scheme::https: {
					return ssl_socket_.lowest_layer().is_open();
				}
			}
			return false;
		}
		inline operator bool() {
			return is_open();
		}

	public:
		// interface to connect & request write
		self_type& operator<<(request &req) {
			connect_write_impl(req);
			return *this;
		}

		// responseを取得
		void read(response &r) {
			read_impl(r);
		}

		void set_deadline_timer() {
			// deadlineをセット
			deadline_.expires_from_now( boost::posix_time::seconds(timeout_) );
			// deadline_timerを開始
			deadline_.async_wait([&](error_code const& ec) {
				// timeoutによる終了(false)のみresultに適用する
				this->on_timeout(ec);
			});
		}

	private:
		void init_ssl() {
			ssl_socket_.set_verify_mode(ssl_traits::verify_peer);
		}

		bool open_socket(ip_value value) {
			error_code ec;
			switch (scheme_id_) {
				case request::scheme::http: {
					socket_.open(value, ec);
					break;
				}
				case request::scheme::https: {
					ssl_socket_.lowest_layer().open(value, ec);
					break;
				}
			}
			if (ec) {
				DEBUG_OUTPUT("    failed to open socket");
				return false;
			}
			DEBUG_OUTPUT("    success to open socket");
			DEBUG_OUTPUT("    set_option<keep_alive>(" << std::boolalpha << keep_alive_ << ")");
			//set_option<typename socket_type::keep_alive>(keep_alive_);
			//set_option<typename socket_type::reuse_address>(keep_alive_);
			return true;
		}
		void close_socket() {
			if (!is_open()) return;

			error_code ec;
			socket_.shutdown(socket_type::shutdown_both, ec);

			if (ec) return;
			socket_.close(ec);
		}

	private:
		// friend function
		// connect
		template<typename C, typename S, typename R>
		friend void network::detail::connect(C&,S&,R&);
		template<typename E, typename C, typename S, typename R>
		friend void network::detail::resolve_handle(error_code const&,E,C&,S&,R&);
		template<typename E, typename C, typename S, typename R>
		friend void network::detail::connect_handle(error_code const&,E,C&,S&,R&);
		// connect_ssl
		template<typename C, typename S, typename R>
		friend void network::detail::connect_ssl(C&,S&,R&);
		template<typename E, typename C, typename S, typename R>
		friend void network::detail::resolve_ssl_handle(error_code const&,E,C&,S&,R&);
		template<typename E, typename C, typename S, typename R>
		friend void network::detail::connect_ssl_handle(error_code const&,E,C&,S&,R&);
		template<typename C, typename S, typename R>
		friend void network::detail::connect_ssl_handshake(error_code const&,C&,S&,R&);
		// write
		template<typename C, typename S, typename R>
		friend void network::detail::write(C&,S&,R&);
		// read
		template<typename C, typename S, typename R>
		friend void network::detail::read(C&,S&,R&);
		template<typename C, typename S, typename R>
		friend void network::detail::read_handle(error_code const& ec,std::size_t,C&,S&,R&);

	private:
		// io_service実行
		void run_impl() {
			// 非同期処理をしない場合、ここで終了を待つ
			if (!work_) {
				DEBUG_OUTPUT("  ios.run() start");
				ios_.run();
				ios_.reset();
				DEBUG_OUTPUT("  ios.run() finish");
			}
		}

		// 通信実行
		void connect_write_impl(request &req) {
			scheme_id_ = req.scheme_id();

			// ヘッダの調査
			check_keepalive(req);  // Keep-Alive設定

			if (!is_keepalive(req)) {
				connect_impl(req);
			}
			else {
				write_impl(req);
			}

			// io_service run();
			run_impl();
		}

		// connect処理
		void connect_impl(request &req) {
			switch (scheme_id_) {
				case request::scheme::http: {
					network::detail::connect(*this, socket_, req);
					break;
				}
				case request::scheme::https: {
					network::detail::connect_ssl(*this, ssl_socket_, req);
					break;
				}
			}

			// deadline_timerをセット
			set_deadline_timer();
		}
		// write処理
		void write_impl(request &req) {
			// async処理の登録
			switch (scheme_id_) {
				case request::scheme::http: {
					// writeへ
					network::detail::write(*this, socket_, req);
					break;
				}
				case request::scheme::https: {
					// writeへ
					network::detail::write(*this, ssl_socket_, req);
					break;
				}
			}

			// deadline_timerをセット
			set_deadline_timer();
		}
		// read処理
		void read_impl(response &r) {
			switch (scheme_id_) {
				case request::scheme::http: {
					// readへ
					network::detail::read(*this, socket_, r);
					break;
				}
				case request::scheme::https: {
					// readへ
					network::detail::read(*this, ssl_socket_, r);
					break;
				}
			}

			// deadline_timerをセット
			set_deadline_timer();

			// io_service run();
			run_impl();
		}

		// 共通処理
		void success() {
			if (!keep_alive_) close_socket();
			// 処理が成功したのでdeadline_timerを止める
			deadline_.cancel();
		}
		// 通信失敗の通知
		void failure() {
			// その他リセット
			close_socket();
			deadline_.cancel();
		}
		// deadline_timerをキャンセル
		void timer_cancel() {
			deadline_.cancel();
		}

	public:// VC用：lambda内でprivateメンバにアクセスできない
		// connectに成功
		template<typename Socket>
		void connect_success(error_code const& ec, Socket &socket, request &req) {
			DEBUG_OUTPUT("    connect_success: ");
			timer_cancel();
			write_impl(req);
		}
		// connectに失敗
		template<typename Socket>
		void connect_failure(error_code const& ec, Socket &socket, request &req) {
			DEBUG_OUTPUT("    connect_failure: ");
			// 初期化
			failure();
		}

		// writeに成功
		void write_success(error_code const& ec, std::size_t bytes_transferred) {
			DEBUG_OUTPUT("    write_success: " << bytes_transferred << "bytes");
			timer_cancel();
		}
		// writeに失敗
		void write_failure(error_code const& ec, std::size_t bytes_transferred) {
			DEBUG_OUTPUT("    write_failure: " << bytes_transferred << "bytes.");
			// 初期化
			failure();
		}

		// readに成功
		void read_success(error_code const& ec, response &resp) {
			DEBUG_OUTPUT("    read_success.");
			// "Connection: close" が返ってきたら
			if (resp.is_close()) {
				keep_alive_ = false;
			}
			// 通信終了処理(成功)
			success();
			// 
			if (work_ && callback_) {
				//callback_(response_buf_);
			}
		}
		// readに失敗
		void read_failure(error_code const& ec, response &resp) {
			DEBUG_OUTPUT("    read_failure: ");
			// 通信終了処理(失敗)
			failure();
		}

		void on_timeout(boost::system::error_code const& ec) {
			// 手動でdeadline_timerがキャンセルされた
			if ( ec == boost::asio::error::operation_aborted ) {
				DEBUG_OUTPUT("  timer manual cancel: ");
				// 手動でキャンセルするのは処理が終了した場合
				// なにもしない
			}
			// 手動キャンセル以外
			else {
				DEBUG_OUTPUT("  on_timeout: ");
				// エラーでないなら、タイムアウト
				if (!ec) {
					//error_message("TIMEOUT!");
				}
				// 何らかのエラー
				else {
					//error_message(boost::system::system_error(error).what());
				}

				// エラーなので処理を中断する
				failure();
			}
		}

		// socketにオプションを設定する
		template<typename Option>
		bool set_option(bool flag) {
			error_code ec;
			Option option(flag);
			switch (scheme_id_) {
				case request::scheme::http: {
					socket_.set_option( option, ec );
					break;
				}
				case request::scheme::https: {
					ssl_socket_.lowest_layer().set_option( option, ec );
					break;
				}
			}
			DEBUG_OUTPUT(
				"set_option(): " << static_cast<bool>(ec)
				<< " : code " << ec.value()
				<< " : message" << ec.message()
			);
			return ec;
		}

/*
		void print_buffer(buffer &buf) {
			std::istream ist(&buf);
			std::string str;
			while ( std::getline(ist, str)  ) {
				std::cout << str << std::endl;
			}
		}
*/

	private:
		// Keep-Alive チェック
		void check_keepalive(request const& req) {
			// Connectionヘッダが設定されている
			auto result = req.headers.find("Connection");
			if ( result != req.headers.end() ) {
				keep_alive_ = (result->second == "Keep-Alive");
			}
			// Connectionが無いときは常にKeep-Alive
			else {
				keep_alive_ = true;
			}
		}
		// Keep-Alive実行中チェック
		//   Keep-Aliveで通信するかどうかを返す
		bool is_keepalive(request const& req) {
			bool is_same_host = (prev_host_ == req.host());
			// 直前に接続したhostを記憶
			prev_host_ = req.host();

			switch (scheme_id_) {
				case request::scheme::http: {
					// socketが閉じている または 直前に接続したhostと異なる場合
					// connectから開始
					if (!socket_.is_open() || !is_same_host) {
						if (socket_.is_open()) socket_.close();
						return false;
					}
					// そうでない場合、socketを再利用してwriteから開始
					else {
						return true;
					}
				}
				case request::scheme::https: {
					// socketが閉じている または 直前に接続したhostと異なる場合
					// connectから開始
					if (!ssl_socket_.lowest_layer().is_open() || !is_same_host) {
						if (ssl_socket_.lowest_layer().is_open()) {
							ssl_socket_.lowest_layer().close();
						}
						return false;
					}
					// そうでない場合、socketを再利用してwriteから開始
					else {
						return true;
					}
				}
			}
			return false;
		}

	private:
		std::shared_ptr<io_service> ios_ptr_;		// io_service実体:
								//   io_serviceが他から渡されなかった場合は自分が実体を持つ
		io_service                  &ios_;		// io_service参照
		std::unique_ptr<work>       work_;		// マルチスレッド時に使う
		resolver                    resolver_;          // resolver type
		socket_type                 socket_;		// http socket
		context                     context_;		// SSL context
		ssl_socket_type             ssl_socket_;	// https socket
		timer_type                  timeout_;		// timeoutまでの時間
		deadline_timer              deadline_;          // deadline timer
		boost::thread               thread_;		// マルチスレッド時に使う
		typename scheme::enum_t     scheme_id_;		// 通信モード http/https

		callback_functor callback_;  // コールバックファンクタ

		string_type prev_host_;  // 直前に接続したhost名

		bool keep_alive_;
	};

	/*
	// interface to read response
	// responseを返す
	template<typename T, typename C>
	basic_client<T, C>& operator=(basic_response<C>& r, basic_client<T, C>& self) {
		self.read(r);
		return self;
	}
	*/

}}}//namespace lulib::network::http

