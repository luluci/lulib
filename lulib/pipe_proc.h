#pragma once

#include <stdlib.h>
#include <string.h>
#include <string>

#include <sys/wait.h>

#ifdef DEBUG
#    include <iostream>
#    define error_message(str) std::cerr << str << std::endl
#else
#    define error_message(str)
#endif

namespace lulib {

	bool pipe_proc(char const* command, char const* input, std::string &output) {
		int pipe_p2c[2];
		int pipe_c2p[2];
		int pid;

		enum enum_t {
			R = 0,
			W = 1,
		};

		if ( ::pipe(pipe_p2c) < 0 ) {
			error_message("failed to open pipe_p2c.");
			return false;
		}
		if ( ::pipe(pipe_c2p) < 0 ) {
			error_message("failed to open pipe_p2c.");
			::close(pipe_p2c[R]);  ::close(pipe_p2c[W]);
			return false;
		}

		if ( (pid = ::fork()) < 0 ) {
			error_message("failed to fork().");
			::close(pipe_p2c[R]);  ::close(pipe_p2c[W]);
			::close(pipe_c2p[R]);  ::close(pipe_c2p[W]);
			return false;
		}

		// child process
		if (pid == 0) {
			::close(pipe_c2p[R]);  ::close(pipe_p2c[W]);

			if (pipe_p2c[R] != STDIN_FILENO) {
				if ( ::dup2(pipe_p2c[R], STDIN_FILENO) != STDIN_FILENO ) {
					error_message("failed to dup2(fd, stdin).");
				}
				::close(pipe_p2c[R]);
			}
			if (pipe_c2p[W] != STDOUT_FILENO) {
				if ( ::dup2(pipe_c2p[W], STDOUT_FILENO) != STDOUT_FILENO ) {
					error_message("failed to dup2(fd, stdout).");
				}
				::close(pipe_c2p[W]);
			}

			if ( ::execlp("sh", "sh", "-c", command, 0) < 0 ) {
				error_message("failed to exec().");
				//return false;
				exit(EXIT_FAILURE);
			}

			return false;
		}
		// parent process
		else {
			const int buff_size = 512;
			char buff[buff_size+1];
			buff[buff_size] = '\0';
			int size;

			::close(pipe_p2c[R]);  ::close(pipe_c2p[W]);

			size = strlen(input);
			if ( ::write(pipe_p2c[W], input, size) != size ) {
				error_message("failed to write to pipe.");
			}
			::close(pipe_p2c[W]);

			while ( (size = ::read(pipe_c2p[R], buff, buff_size)) > 0 ) {
				output.append(buff, size);
			}
			if (size < 0) {
				error_message("failed to read from pipe.");
			}
			else if (size == 0) {
				error_message("child closed pipe: " << command << " < " << input);
			}
			::close(pipe_c2p[R]);

			int status = 0;
			::waitpid(pid, &status, 0);

			return true;
		}

		return true;
	}

}// namespace lulib
