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
#include "PollingHelper.h"

//#include <sys/ioctl.h>
//#include <sys/poll.h>
//#include <sys/socket.h>
//#include <sys/time.h>
//#include <sys/un.h>
//#include <netinet/in.h>
//#include <errno.h>
//#include <unistd.h>
//#include <iostream>
//#include <string.h>

//#include <memory>
//#include <vector>
//#include <unordered_map>

namespace network {

	class AsyncUnixServer: public AbstractServer {
	private:
		UnixServerSocket::ptr _socket;
		size_t _max_clients; //maximum of allowed clients
		PollingHelper<UnixClientSocket> _polling;
	public:

		AsyncUnixServer() : AbstractServer(), _socket(std::make_shared<UnixServerSocket>()), _max_clients(32) {

		};

		void listen(std::string_view port) override {
			set_port(port);

			_socket->init({}, port);

			int status = ::listen(_socket->get_socket(), _max_clients);

			if (status < 0) {
				throw std::runtime_error("Unable to start listening socket. Error: " + strerr());
			}

			set_listen();

			//initializing polling
			_polling.add_client(_socket);

			int timeout = -1; //timeout in ms, -1 - infinite
			int nfds = 1; //number of descriptors

			socklen_t client_len = 0;
			struct sockaddr client_address;
			int client_sockfd = 0;

			auto&& poll_fds = _polling.get_polling();

			while (true) {
				LOG_INFO("Waiting for client connection... Total: " << _polling.get_size() - 1);

				status = poll(&poll_fds[0], poll_fds.size(), timeout);

				if (status < 0) {
					LOG_ERROR("Poll failed. Error: " << strerror(errno));
					break;
				}

				std::vector<pollfd>::iterator it;
				auto&& end = poll_fds.end();
		        for (it = poll_fds.begin(); it != end; ++it) {

					if (it->revents & POLLIN) {
						//accept connection on listening socket
						if(it->fd == _socket->get_socket()) {
							client_len = sizeof(client_address);
							client_sockfd = accept(_socket->get_socket(), &client_address, &client_len);

							if (_max_clients <= _polling.get_size() - 1) {
								LOG_INFO("Maximum number of allowed clients is reached. Max: " << _max_clients);
								::close(client_sockfd);
							} else {
								_polling.add_client(client_sockfd, client_address);

								auto&& ucs =  _polling.get_last_client();

								LOG_INFO(ucs->get_host() << ":" << ucs->get_port() << " connected, socket: " << ucs->get_socket());
							}
						//it is not listening socket therefore an existing connection - do IO operations
						} else {
							auto&& client = _polling.get_client(it->fd);

							bool close_connection = false;

							while (true) {
								char buffer[80];

								ssize_t status = recv(it->fd, &buffer, sizeof(buffer), 0);

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
									status = send(it->fd, buffer, size, 0);

									if (status < 0) {
										LOG_ERROR("Send failed. Error: " << strerr());
										close_connection = true;
										break;
									} else {
										LOG_DEBUG("writing to client on fd " << it->fd << ", write size: " << size);
									}
								}
							}

							if (close_connection) {
							   LOG_DEBUG("Removing client. Socket: " << it->fd);
							   _polling.delete_client(it);
							}
						}
					}
				}
			}
		};

		void stop() override {
			if (is_listening()) {
				_socket->shutdown();
			}
		};

		virtual ~AsyncUnixServer() {
			try {
				stop();
			} catch (const std::exception& e) {
				LOG_ERROR(e.what());
			}

			_socket->close();
		};
	};

}

#endif /* SRC_ASYNCUNIXSOCKET_H_ */
#endif //_WIN32
