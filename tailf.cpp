// This program approximates the functionality of tail -f using inotify.
// I left in the commented-out garbage because it's somewhat interesting.

#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>

int main(int argc, char **argv) {
	assert(argc == 2);
	// std::ifstream stream(argv[1]);
	// for (;;) {
	// 	char ch = '\0';
	// 	if (stream >> ch)
	// 		std::cout << '\'' << ch << "' (" << static_cast<int>(ch) << ")\n";
	// }

	int fd = open(argv[1], O_RDONLY);
	assert(fd != -1);

	ssize_t status;

	int ifd = inotify_init();
	assert(ifd != -1);
	inotify_add_watch(ifd, argv[1], IN_MODIFY);

	pollfd pfd {.fd = ifd, .events = POLLIN, .revents = 0};

	inotify_event events[256];

	while (0 <= (status = poll(&pfd, 1, -1))) {
		std::cout << "status[" << status << "] revents[" << pfd.revents << "]\n";
		if (0 < status) {
			ssize_t rstatus = read(ifd, events, sizeof(events));
			char buf[256] {};

			do {
				rstatus = read(fd, buf, sizeof(buf));
				assert(rstatus != -1);
				for (ssize_t i = 0; i < rstatus; ++i) {
					char ch = buf[i];
					std::cout << '\'' << ch << "' (" << static_cast<int>(ch) << ')' << std::endl;
				}
				// else
					// 	std::cout << "!!!" << std::endl;
			} while (0 < rstatus);
		}
	}

	std::cout << "Done.\n";










	// pollfd pfd {.fd = fd, .events = POLLIN, .revents = 0};

	// std::cout << std::endl << std::endl << "\e[31mpfd.fd[" << pfd.fd << "]\e[39m" << std::endl;

	// while (0 <= (status = poll(&pfd, 1, -1))) {
	// 	// std::cout << "status[" << status << "]" << std::endl;
	// 	if (0 < status) {
	// 		char ch;
	// 		ssize_t rstatus = read(fd, &ch, 1);
	// 		assert(rstatus != -1);
	// 		if (0 < rstatus)
	// 			std::cout << '\'' << ch << "' (" << static_cast<int>(ch) << ')' << std::endl;
	// 		// else
	// 		// 	std::cout << "!!!" << std::endl;
	// 	}
	// }




	// fd_set fds;
	// FD_ZERO(&fds);
	// FD_SET(fd, &fds);


	// timeval tv;
	// tv.tv_sec = 10;
	// tv.tv_usec = 0;

	// while (0 <= (status = select(fd + 1, &fds, nullptr, nullptr, &tv))) {
	// 	FD_ZERO(&fds);
	// 	FD_SET(fd, &fds);
	// 	tv.tv_sec = 10;
	// 	tv.tv_usec = 0;
	// 	std::cout << "status[" << status << "] sec[" << tv.tv_sec << "] usec[" << tv.tv_usec << "]\n";
	// 	if (0 < status) {
	// 		char ch;
	// 		ssize_t rstatus = read(fd, &ch, 1);
	// 		assert(rstatus != -1);
	// 		if (0 < rstatus)
	// 			std::cout << '\'' << ch << "' (" << static_cast<int>(ch) << ")\n";
	// 	}
	// }

	// std::cout << "Done.\n";


	// for (;;) {
	// 	char ch;
	// 	ssize_t status = read(fd, &ch, 1);
	// 	assert(status != -1);
	// 	if (0 < status)
	// 		std::cout << '\'' << ch << "' (" << static_cast<int>(ch) << ")\n";
	// 	// Should be using select()
	// }
}
