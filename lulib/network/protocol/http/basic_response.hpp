#pragma once
#pragma warning(disable : 4819)

#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include <lulib/lexical_cast.hpp>
#include <lulib/type_traits/char_traits.hpp>

#include <lulib/network/detail/read.hpp>
#include <lulib/network/protocol/http/detail/parse_response.hpp>
#include <lulib/network/protocol/http/detail/header_container.hpp>

namespace lulib { namespace network { namespace http {

	template<typename T, typename C> class basic_client;

	template<typename Char>
	class basic_response {
		typedef basic_response<Char> self_type;

	public:
		// CharTraits
		typedef lulib::char_traits<Char> char_traits;
		typedef typename char_traits::char_type char_type;
		typedef typename char_traits::string_type string_type;
		// HeaderContainer
		typedef typename detail::header_container<Char>::type header_container;
		// buffer type
		typedef boost::asio::streambuf buffer_type;

		// response読み込み状況
		struct state {
			enum enum_t {
				header,              // header読み込み中
				body,                // body(通常)読み込み中
				body_chunk,          // body(chunk)読み込み中
				body_chunk_trailer,  // body(chunk, trialer)読み込み中
				eof,                 // 読み込み完了
			};
		};

	public:
		basic_response()
		: buffer_(), version_(), status_code_(0), status_(), header_(), body_()
		, content_length_(0), chunk_rest_(0), state_(state::header), is_chunked_(false)
		{
			
		}
		basic_response(self_type &&obj) {
			//buffer_ = std::move(obj.buffer_);
			version_ = std::move(obj.version_);
			status_code_ = obj.status_code_;
			status_ = std::move(obj.status_);
			header_ = std::move(obj.header_);
			body_ = std::move(obj.body_);
			content_length_ = obj.content_length_;
			chunk_rest_ = obj.chunk_rest_;
			state_ = obj.state_;
			is_chunked_ = obj.is_chunked_;
		}
		template<typename T, typename C>
		basic_response(basic_client<T,C>& client)
		: version_(), status_code_(0), status_(), header_(), body_()
		, content_length_(0), chunk_rest_(0), state_(state::header), is_chunked_(false)
		{
			client.read(*this);
		}
		template<typename T, typename C>
		self_type& operator=(basic_client<T,C> &client) {
			client.read(*this);
		}


		// response body
		string_type const& body() {
			return body_;
		}

		// responseの読み込みが終わったか否か
		operator bool() const {
			return state_ == state::eof;
		}
		bool operator!() const {
			return state_ != state::eof;
		}

		void reset() {
			version_.clear();
			status_code_ = 0;
			status_.clear();
			header_.clear();
			body_.clear();
			content_length_ = 0;
			chunk_rest_ = 0;
			state_ = state::header;
			is_chunked_ = false;
		}

		// Transfer-Encodingがchunkedかどうか
		bool is_chunked() { return is_chunked_; }

	public:
		template<typename Buffer>
		self_type& operator<<(Buffer &buffer) {
			parse(buffer);
			return *this;
		}

		void parse() {
			parse(buffer_);
		}
		template<typename Buffer>
		void parse(Buffer &buffer) {
			switch (state_) {
				// header読み込み
				case state::header: {
					get_header(buffer);
					break;
				}
				// body読み込み
				case state::body:
				case state::body_chunk:
				case state::body_chunk_trailer: {
					get_body(buffer);
					break;
				}
				// 読み込み完了(受け付けない)
				case state::eof: {
					break;
				}
			}
		}

	public:
		void version(string_type const& version) {
			version_ = version;
		}
		void status_code(std::size_t code) {
			status_code_ = code;
		}
		void status(string_type const& status) {
			status_ = status;
		}
		void header(string_type const& key, string_type const& value) {
			header_.insert( typename header_container::value_type(key, value) );
		}

		// bufferを渡す
		buffer_type& buffer() { return buffer_; }
		// bufferにデータを書き込んだことの通知を受ける
		void read_success() {
			parse();
		}

	private:
		// read
		template<typename C, typename S, typename R>
		friend void network::detail::read(C&,S&,R&);
		template<typename C, typename S, typename R>
		friend void network::detail::read_handle(error_code const& ec,std::size_t,C&,S&,R&);

		// parse free function
		template<typename C, typename I>
		friend bool detail::parse_response_header(C&,I&,I);
		template<typename C, typename I>
		friend bool detail::parse_response_body_chunked(C&,I&,I);
		template<typename C, typename I>
		friend bool detail::parse_response_body_chunked_chunk(C&,I&,I);
		template<typename C, typename I>
		friend bool detail::parse_response_body_chunked_chunk_size(C&,I&,I);
		template<typename C, typename I>
		friend bool detail::parse_response_body_chunked_chunk_data(C&,I&,I);
		template<typename C, typename I>
		friend bool detail::parse_response_body_chunked_trailer(C&,I&,I);

		// bufferからheaderを取得
		// streambuf
		//   return: 消費したバッファサイズ
		std::size_t get_header(buffer_type &buffer) {
			auto cb = buffer.data();            // const_buffers_typeを取得
			std::size_t size = get_header(cb);  // 共通処理
			buffer.consume(size);
			return size;
		}
		// other
		template<typename Buffer>
		std::size_t get_header(Buffer &buffer) {
			auto it = buffers_begin(buffer);
			auto end = buffers_end(buffer);
			// parse header
			return get_header(it, end);
		}
		template<typename Iterator>
		std::size_t get_header(Iterator &it, Iterator end) {
			// iterator初期位置
			Iterator begin = it;
			// parse開始
			// itはレスポンスボディの直前までforwardingされる
			bool result = detail::parse_response_header(*this, it, end);
			// http response headerのパース成功
			if (result) {
				// chunk判定
				if (header_["Transfer-Encoding"] == "chunked") is_chunked_ = true;
				// chunkでないなら
				else content_length_ = lulib::lexical_cast<lulib::as_uint>(
						header_["Content-Length"]
				);

				// body: Transfer-Encoding
				state_ = is_chunked_ ? state::body_chunk : state::body;
				// 残ったデータの処理
				get_body(it, end);
			}
			return (it - begin);
		}

		// bufferからbodyを取得
		// streambuf
		std::size_t get_body(buffer_type &buffer) {
			auto cb = buffer.data();          // const_buffers_typeを取得
			std::size_t size = get_body(cb);  // 共通処理
			buffer.consume(size);
			return size;
		}
		// other
		template<typename Buffer>
		std::size_t get_body(Buffer &buffer) {
			auto it = buffers_begin(buffer);
			auto end = buffers_end(buffer);
			return get_body(it, end);
		}
		template<typename Iterator>
		std::size_t get_body(Iterator &it, Iterator end) {
			// iterator初期位置
			Iterator begin = it;
			// body取得
			switch (state_) {
				// chunked
				case state::body_chunk:
				case state::body_chunk_trailer: {
					get_body_chunk(it, end);
					break;
				}
				// 通常
				case state::body: {
					get_body_default(it, end);
					break;
				}
			}
			return (it - begin);
		}
		// default
		template<typename Iterator>
		void get_body_default(Iterator &it, Iterator end) {
			// バッファサイズを取得
			std::size_t buff_size = (end - it);
			// バッファサイズ+取得済みbodyサイズが残りContent-Lengthを溢れる場合
			if ((buff_size + body_.size()) > content_length_) {
				std::size_t diff = content_length_ - body_.size();
				body_.append(it, (it+diff));  // Content-Length分だけ追加
				it += diff;                   // iterator更新
			}
			// Content-Length以下のとき
			else {
				body_.append(it, end);  // 追加
				it = end;               // iterator更新
			}

			// bodyがContent-Lengthになった
			if (body_.size() >= content_length_) {
				state_ = state::eof;            // 読み込み終了
			}
		}
		// chunked
		template<typename Iterator>
		void get_body_chunk(Iterator &it, Iterator end) {
			detail::parse_response_body_chunked(*this, it, end);
		}

	private:
		buffer_type buffer_;       // buffer

		string_type version_;      // HTTP version
		std::size_t status_code_;  // status code
		string_type status_;       // status description
		header_container header_;  // response header
		string_type body_;         // response body

		// other
		std::size_t content_length_;       // bodyサイズ
		std::size_t chunk_rest_;           // chunkデータ取得残りサイズ
		typename state::enum_t state_;
		bool is_chunked_;          // chunked判定
	};

}}}//namespace lulib::network::http

