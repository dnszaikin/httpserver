/*
 * UnixServerSocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_UNIXSERVERSOCKET_H_
#define POLLHTTPD_UNIXSERVERSOCKET_H_

#ifndef _WIN32
#include "AbstractSocket.h"
#include "Logger.h"
#include <memory>
#include "CommonUtils.h"

namespace dnszaikin::pollhttpd::network {

	struct handle_deleter
	{
		void operator()(addrinfo* handle) const
		{
			freeaddrinfo(handle);
		}
	};

	class UnixServerSocket: public AbstractSocket {
	private:
		std::unique_ptr<addrinfo, handle_deleter> _addr;
	public:
		UnixServerSocket(): AbstractSocket() {};

		void init(std::string_view host, std::string_view port) override {
			AbstractSocket::init(host, port);

			struct addrinfo hints;
			memset(&hints, 0, sizeof(hints));

			hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6
			hints.ai_socktype = SOCK_STREAM; // TCP Socket stream
			hints.ai_flags = AI_PASSIVE;

			struct addrinfo* temp;

			int status = getaddrinfo(NULL, std::string(get_port()).c_str(), &hints, &temp);

			_addr.reset(temp);

			if (status < 0) {
				throw std::runtime_error("Unable to translate service name to socket address. Error: " + std::string(gai_strerror(status)));
			}

			int socket_fd = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);

			if (socket_fd < 0) {
				throw std::runtime_error("Unable to create socket. Error: " + utils::network::strerr());
			}

			set_socket(socket_fd);

			//enable address reuse
			int enable = 1;
			status = setsockopt(get_socket(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

			if (status < 0) {
				throw std::runtime_error("Setting reuse address failed. Error: " + utils::network::strerr());
			}

			int opt = 1;

			//enable non-blocking IO, all client sockets inherits this state from listening socket
			status = ioctl(get_socket(), FIONBIO, &opt);

			if (status < 0) {
				throw std::runtime_error("Failed to set non-blocking mode. Error: " + utils::network::strerr());
			}

			status = bind(get_socket(), _addr->ai_addr, _addr->ai_addrlen);

			if (status < 0) {
				throw std::runtime_error("Failed to bind socket. Error: " + utils::network::strerr());
			}

			set_connected();
		}

		virtual ~UnixServerSocket() {

		};

		callback begin_recv() override { return std::bind(&AbstractSocket::end_recv, this, false, 0); }
		callback begin_send() override { return std::bind(&AbstractSocket::end_send, this, false, 0); }
		void end_recv(bool, size_t) override {}
		void end_send(bool, size_t) override {}

	};
}

#endif /* SRC_UNIXSERVERSOCKET_H_ */
#endif //WIN32
