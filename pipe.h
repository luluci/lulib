#pragma once

/*
//
Cygwin, g++ 4.5.1

// sample code
lulib::pipe pipe("head -n 2 | tail -n 1");

char const* input = "line1\nline2\nline3";
pipe.write(input, strlen(input));

std::string ret = pipe.read();
std::cout << "ret: " << ret << std::endl;

// result
ret: line2
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include <string>

namespace lulib {

	class pipe {
		// pipe file descriptor
		class pipe_fd {
			struct mode {
				enum enum_t {
					read = 0,
					write = 1,
					size,
				};
			};

		public:
			pipe_fd() {}
			~pipe_fd() {
				close();
			}

			bool open() {
				valid_read_ = valid_write_ = ( ::pipe(fd_) == 0 );  // pipe作成
				return valid_read_;
			}
			// 手動close
			void close() {
				// pipeを閉じる
				close_read();
				close_write();
			}
			void close_read() {
				// read
				if (valid_read_) {
					::close(fd_[mode::read]);
					valid_read_ = false;
				}
			}
			void close_write() {
				// write
				if (valid_write_) {
					::close(fd_[mode::write]);
					valid_write_ = false;
				}
			}

			// file descriptor: read
			inline int read() {
				return fd_[mode::read];
			}
			// file descriptor: write
			inline int write() {
				return fd_[mode::write];
			}

			inline operator bool() {
				return valid_read_ && valid_write_;
			}

		private:
			bool valid_read_, valid_write_;  // fd is valid?
			int fd_[mode::size];  // fd
		};

	public:
		// default ctor
		pipe() : valid_(false), pfd_p2c_(), pfd_c2p_(), child_pid_(-1) {
		}
		pipe(std::string const& command) : pfd_p2c_(), pfd_c2p_(), child_pid_(-1) {
			valid_ = open(command.c_str());
		}
		pipe(char const* command) : pfd_p2c_(), pfd_c2p_(), child_pid_(-1) {
			valid_ = open(command);
		}
		~pipe() {
			close();
		}

		void wait_pid() {
			if (child_pid_ != -1) {
				int status = 0;

				::waitpid(child_pid_, &status, 0);

				child_pid_ = -1;
			}
		}

		// 手動close
		void close() {
			pfd_p2c_.close();
			pfd_c2p_.close();

			::dup2(org_fd_stdin_, 0);
			::dup2(org_fd_stdout_, 1);

			// 子プロセスの終了を待つ
			wait_pid();
		}
		// 手動open
		bool open(char const* command) {
			if (pfd_p2c_ || pfd_c2p_) {
				// 2重openはエラー
				return false;
			}

			// パイプ作成
			pfd_p2c_.open();
			if (!pfd_p2c_) return false;
			pfd_c2p_.open();
			if (!pfd_c2p_) return false;

			// デフォルトのファイルディスクリプタを記憶しておく
			org_fd_stdin_  = ::dup( 0 );
			org_fd_stdout_ = ::dup( 1 );

			// invoke process: 自分の複製を作成
			child_pid_ = ::fork();
			// pidが負数だと失敗
			if (child_pid_ < 0) return false;

			// pidが0だと、自分は複製された子プロセス
			// 子プロセスが担当する処理
			if (child_pid_ == 0) {
				// 親プロセスが強制的に元に戻すのでエラーチェックはしない
				// 標準入力をパイプに切り替える
				::dup2(pfd_p2c_.read(), 0);
				// 標準出力をパイプに切り替える
				::dup2(pfd_c2p_.write(), 1);

				// パイプはもう閉じる
				pfd_p2c_.close();
				pfd_c2p_.close();

				// コマンドを実行
				// execlpは実行に成功したら返ってこない
				// 失敗したら手動で自殺
				if ( ::execlp( "sh", "sh", "-c", command, NULL) < 0) exit(EXIT_FAILURE);

			}
			// pidが0より大きいと、自分は親プロセスで、pidは子プロセスのpid
		}

		// c style
		int write(char const* str, int str_len) {
			return ::write( pfd_p2c_.write(), str, str_len );
		}
		int read(char *buff, int buff_size) {
			pfd_p2c_.close_write();
			pfd_c2p_.close_write();
			return ::read( pfd_c2p_.read(), buff, buff_size );
		}

		// C++
		void write(std::string const& str) {
			::write( pfd_p2c_.write(), str.c_str(), str.size() );
		}
		std::string read() {
			pfd_p2c_.close_write();
			pfd_c2p_.close_write();

			const int buff_size = 512;
			char buff[buff_size];

			std::string str;
			int size;
			while ( (size = ::read( pfd_c2p_.read(), buff, buff_size )) > 0 ) {
				str.append(buff, size);
			}

			pfd_c2p_.close_read();

			// 子プロセスの終了を待つ
			wait_pid();

			return str;
		}

	private:
		bool valid_;
		pipe_fd pfd_p2c_, pfd_c2p_;
		int org_fd_stdin_, org_fd_stdout_;  // デフォルトのファイルディスクリプション
		int child_pid_;
	};

}// namespace mylib
