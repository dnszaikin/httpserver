/*
 * UnixSocket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_UNIXSOCKET_H_
#define SRC_UNIXSOCKET_H_

#include "AbstractSocket.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

#include "Logger.h"
#include <memory>

namespace network {

	std::string strerr() {
		return std::string(strerror(errno));
	}

	struct handle_deleter
	{
		void operator()(addrinfo* handle) const
		{
			freeaddrinfo(handle);
		}
	};

	class UnixSocket: public AbstractSocket {
	private:
		int _socket;
		std::unique_ptr<struct addrinfo, handle_deleter> _addr;

		struct pollfd poll_fds[32]; //Poll size is 32

	public:

		UnixSocket() : AbstractSocket(), _socket(-1)	{

		};

		void listen(std::string_view port) override {
			set_port(port);

			struct addrinfo hints;
			memset(&hints, 0, sizeof(hints));

			hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6
			hints.ai_socktype = SOCK_STREAM; // TCP Socket stream
			hints.ai_flags = AI_PASSIVE;

			struct addrinfo* temp;

			int status = getaddrinfo(NULL, get_port().c_str(), &hints, &temp);

			_addr.reset(temp);

			if (status < 0) {
				throw std::runtime_error("Unable to translate service name to socket address. Error: " + std::string(gai_strerror(status)));
			}

			_socket = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);

			if (_socket < 0) {
				throw std::runtime_error("Unable to create socket. Error: " + strerr());
			}

			//enable address reuse
			int enable = 1;
			status = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

			if (status < 0) {
				throw std::runtime_error("Setting reuse address failed. Error: " + strerr());
			}

			int opt = 1;

			//enable non-blocking IO
			status = ioctl(_socket, FIONBIO, &opt);

			if (status < 0) {
				throw std::runtime_error("Failed to set non-blocking mode. Error: " + strerr());
			}

			status = bind(_socket, _addr->ai_addr, _addr->ai_addrlen);

			if (status < 0) {
				throw std::runtime_error("Failed to bind socket. Error: " + strerr());
			}

			status = ::listen(_socket, 32);

			if (status < 0) {
				throw std::runtime_error("Unable to start listening socket. Error: " + strerr());
			}

			set_listen();

			//initializing polling
			memset(poll_fds, 0 , sizeof(poll_fds));

			poll_fds[0].fd = _socket;
			poll_fds[0].events = POLLIN;

			int timeout = 36*60*1000; //timeout in ms
			int nfds = 1;

			int client_len = 0;
			struct sockaddr_un client_address;
			int client_sockfd = 0;

			while (true) {
				char ch;
				int nread;

				LOG_INFO("Waiting for client connection... Total: " << nfds - 1);

				status = poll(poll_fds, nfds, timeout);

				if (status < 0) {
					LOG_ERROR("Poll failed. Error: " << strerror(errno));
					break;
				}

				for(int fd_index = 0; fd_index < nfds; fd_index++) {
					if( poll_fds[fd_index].revents & POLLIN ) {
						if(poll_fds[fd_index].fd == _socket) {
							client_len = sizeof(client_address);
							client_sockfd = accept(_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_len);


							poll_fds[nfds].fd = client_sockfd;
							poll_fds[nfds].events = POLLIN;
							nfds++;


							printf("Adding client on fd %d\n", client_sockfd);
						}
						else {
							ioctl(poll_fds[fd_index].fd, FIONREAD, &nread);


							if( nread == 0 )
							{
								::close(poll_fds[fd_index].fd);
								poll_fds[fd_index].events = 0;
								printf("Removing client on fd %d\n", poll_fds[fd_index].fd);
								int i = fd_index;
								//for( i = fd_index; i<numfds; i++)
								//{
								poll_fds[i] = poll_fds[i + 1];
								//}
								nfds--;
							}


							else {
								ssize_t size= read(poll_fds[fd_index].fd, &ch, 1);
								LOG_DEBUG("reading from client on fd " << poll_fds[fd_index].fd << ", read size: " << size);
								ch++;
								size = write(poll_fds[fd_index].fd, &ch, 1);
								LOG_DEBUG("writting to client on fd " << poll_fds[fd_index].fd << ", write size: " << size);
							}
						}
					}
				}
			}
		};

		void close() override {
			if (is_listening()) {
				int status = shutdown(_socket, SHUT_RDWR);

				if (status < 0) {
					throw std::runtime_error("Unable to shutdown socket. Error: " + strerr());
				}
			}
		};

		virtual ~UnixSocket() {
			try {
				close();
			} catch (const std::exception& e) {
				LOG_ERROR(e.what());
			}

			int status = ::close(_socket);

			if (!status) {
				LOG_ERROR("Failed to close socket. Error: " << strerr());
			}
		};
	};

}

#endif /* SRC_UNIXSOCKET_H_ */
#endif //_WIN32
