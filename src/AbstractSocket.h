/*
 * AbstractSocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ABSTRACTSOCKET_H_
#define SRC_ABSTRACTSOCKET_H_

#include "ISocket.h"
#include "NetworkUtils.h"
#include "Logger.h"

#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

namespace network {

	class AbstractSocket: public network::ISocket {
	private:
		std::string _host;
		std::string _port;
		int _socket;
	public:
		AbstractSocket(): _host{}, _port{}, _socket(-1) {};

		void init(std::string_view, std::string_view) override {};
		void init(std::string_view, std::string_view, int) override {};

		void shutdown() override {
			int status = ::shutdown(get_socket(), SHUT_RDWR);

			if (status < 0) {
				throw std::runtime_error("Unable to shutdown socket. Error: " + strerr());
			}
		}

		void close() override {
			int status = ::close(get_socket());

			if (!status) {
				LOG_ERROR("Failed to close socket. Error: " << strerr());
			}
		}

		/*
		 * get IP Port
		 */
		std::string_view get_port() const override {
			return _port;
		}

		/*
		 * get host name
		 */
		std::string_view get_host() const override {
			return _host;
		}

		/*
		 * returns socket descriptor
		 */
		int get_socket() const override {
			return _socket;
		}

		void set_port(std::string_view port) {
			_port = port;
		}

		void set_host(std::string_view host) {
			_host = host;
		}

		void set_socket(int socket) {
			_socket = socket;
		}

		virtual ~AbstractSocket() {};
	};
}

#endif /* SRC_ABSTRACTSOCKET_H_ */
