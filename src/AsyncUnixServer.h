/*
 * UnixSocket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_ASYNCUNIXSOCKET_H_
#define SRC_ASYNCUNIXSOCKET_H_

#include "AbstractServer.h"
#include "UnixServerSocket.h"
#include "UnixClientSocket.h"
#include "Logger.h"
#include "NetworkUtils.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

#include <memory>
#include <vector>

namespace network {

	struct poll_item {
		struct pollfd poll_fd;
		UnixClientSocket socket_fd;

		poll_item() {
			memset(&poll_fd, 0 , sizeof(poll_fd));
		};
	} poll_item_t;

	class UnixServerSocket;
	class AsyncUnixServer: public AbstractServer {
	private:
		UnixServerSocket _socket;
		struct pollfd poll_fds[32]; //Poll size is 32

		std::vector<poll_item_t> poll_items;

	public:

		AsyncUnixServer() : AbstractServer() {

		};

		void listen(std::string_view port) override {
			set_port(port);

			_socket.init({}, port);

			int status = bind(_socket.get_socket(), _socket.get_addr().ai_addr, _socket.get_addr().ai_addrlen);

			if (status < 0) {
				throw std::runtime_error("Failed to bind socket. Error: " + strerr());
			}

			status = ::listen(_socket.get_socket(), 32); //32 is queue length, if 32 is reached client got error

			if (status < 0) {
				throw std::runtime_error("Unable to start listening socket. Error: " + strerr());
			}

			set_listen();

			poll_items.reserve(32);
			auto&& it = poll_items.front();

			for (int i=1; i < 32; ++i) {

			}

			//initializing polling
			memset(poll_fds, 0 , sizeof(poll_fds));

			poll_fds[0].fd = _socket.get_socket();
			poll_fds[0].events = POLLIN;

			int timeout = -1; //timeout in ms, -1 - infinite
			int nfds = 1; //number of descriptors

			socklen_t client_len = 0;
			struct sockaddr client_address;
			int client_sockfd = 0;
			int opt = 1; //ioctl enable flag
			while (true) {
				LOG_INFO("Waiting for client connection... Total: " << nfds - 1);

				status = poll(poll_fds, nfds, timeout);

				if (status < 0) {
					LOG_ERROR("Poll failed. Error: " << strerror(errno));
					break;
				}

				for(int fd_index = 0; fd_index < nfds; fd_index++) {

					if (poll_fds[fd_index].revents & POLLIN) {

						//accept connection on listening socket
						if(poll_fds[fd_index].fd == _socket.get_socket()) {
							client_len = sizeof(client_address);
							client_sockfd = accept(_socket.get_socket(), &client_address, &client_len);

							poll_fds[nfds].fd = client_sockfd;
							poll_fds[nfds].events = POLLIN;
							nfds++;

							auto&& ret = get_name_info(client_address);

							UnixClientSocket clsckt;
							clsckt.init(ret.first, ret.second, client_sockfd);

							LOG_INFO(clsckt.get_host() << ":" << clsckt.get_port() << " connected, socket: " << clsckt.get_socket());

						//it is not listening socket therefore an existing connection - do IO operations
						} else {
							bool close_connection = false;

							while (true) {
								char buffer[80];

								ssize_t status = recv(poll_fds[fd_index].fd, &buffer, sizeof(buffer), 0);

								//error when read
								if (status < 0) {
								   if (errno != EWOULDBLOCK) {
									   LOG_ERROR("Recv failed. Error: " << strerr());
									   close_connection = true;
								   }
								   break;
								}
								//client disconnected
								else if (status == 0) {
									close_connection = true;
									LOG_INFO("Client disconnected.");
									break;
								}
								//do read
								else {
									ssize_t size = status;
									LOG_DEBUG("Received bytes: " << size);
									status = send(poll_fds[fd_index].fd, buffer, size, 0);

									if (status < 0) {
										LOG_ERROR("Send failed. Error: " << strerr());
										close_connection = true;
										break;
									} else {
										LOG_DEBUG("writing to client on fd " << poll_fds[fd_index].fd << ", write size: " << size);
									}
								}
							}

							if (close_connection) {
							   ::close(poll_fds[fd_index].fd);
							   LOG_INFO("Removing client on socket: " << poll_fds[fd_index].fd);
							   poll_fds[fd_index].fd = -1;
							   int i = fd_index;
							   poll_fds[i] = poll_fds[i + 1];
							   nfds--;
							}
						}
					}
				}
			}
		};

		void stop() override {
			if (is_listening()) {
				_socket.shutdown();
			}
		};

		virtual ~AsyncUnixServer() {
			try {
				stop();
			} catch (const std::exception& e) {
				LOG_ERROR(e.what());
			}

			_socket.close();
		};
	};

}

#endif /* SRC_ASYNCUNIXSOCKET_H_ */
#endif //_WIN32
