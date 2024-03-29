/*
 * AsyncUnixServer.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_ASYNCUNIXSOCKET_H_
#define SRC_ASYNCUNIXSOCKET_H_

#include <future>
#include <functional>

#include "AbstractServer.h"
#include "UnixServerSocket.h"
#include "UnixClientSocket.h"
#include "Logger.h"
#include "PollingHelper.h"
#include "Types.h"

namespace dnszaikin::pollhttpd::network {

	template <class T>
	class AsyncUnixServer: public AbstractServer {
	private:
		std::shared_ptr<UnixServerSocket> _socket;
		size_t _max_clients; //maximum of allowed clients
		PollingHelper<T> _polling;
		std::shared_ptr<IHandlerFactory> _handler;
	public:

		AsyncUnixServer() : AbstractServer(), _socket(std::make_shared<UnixServerSocket>()), _max_clients(32) {

		};

		int get_socket() override { return _socket->get_socket();}

		const std::unordered_map<int, ISocket::ptr>& get_clients() {
			return _polling.get_clients();
		}

		void set_handler_factory(std::shared_ptr<IHandlerFactory> handler) {
			_handler = handler;
		}

		void listen(std::string_view port) override {
			set_port(port);

			_socket->init({}, port);

			int status = ::listen(_socket->get_socket(), _max_clients);

			if (status < 0) {
				throw std::runtime_error("Unable to start listening socket. Error: " + utils::network::strerr());
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

			while (is_listening()) {
				LOG_INFO("Waiting for client connection... Total: " << _polling.get_size() - 1);

				status = poll(&poll_fds[0], poll_fds.size(), timeout);

				if (status < 0) {
					LOG_ERROR("Poll failed. Error: " << strerror(errno));
					break;
				}

				std::vector<pollfd>::iterator it;
				auto&& end = poll_fds.end();
		        for (it = poll_fds.begin(); it != end; ++it) {
					//accept connection on listening socket
					if(it->fd == _socket->get_socket()) {
						if (it->revents & POLLIN) {
							client_len = sizeof(client_address);
							client_sockfd = accept(_socket->get_socket(), &client_address, &client_len);

							if (_max_clients <= _polling.get_size() - 1) {
								LOG_INFO("Maximum number of allowed clients is reached. Max: " << _max_clients);
								::close(client_sockfd);
							} else {
								_polling.add_client(client_sockfd, client_address, _handler);

								auto&& ucs =  _polling.get_last_client();

								LOG_INFO(ucs->get_host() << ":" << ucs->get_port() << " connected, socket: " << ucs->get_socket());
							}
						}
					//it is not listening socket therefore an existing connection - do IO operations
					} else {
						auto&& client = _polling.get_client(it->fd);
						if (client) {
							if (it->revents & POLLIN) {

								auto end_recv = client->begin_recv();
								auto end_send = client->begin_send();

								std::async(end_recv);
								std::async(end_send);

							}

							if (!client->is_connected()) {
								_polling.delete_client(it);
							}
						}
					}
				}
			}
		};

		void stop() override {
			reset_listen();
			_socket->shutdown();
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
