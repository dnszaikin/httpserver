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
		bool _is_connected;
	public:
		AbstractSocket(): _host{}, _port{}, _socket(-1), _is_connected(false) {};

		void init(std::string_view, std::string_view) override {};
		void init(std::string_view, std::string_view, int) override {};

		void shutdown() override {
			_is_connected = false;

			int status = ::shutdown(get_socket(), SHUT_RDWR);

			if (status < 0) {
				throw std::runtime_error("Unable to shutdown socket. Error: " + strerr());
			}
		}

		virtual void set_connected() {
			_is_connected = true;
		}

		bool is_connected() const override {
			return _is_connected;
		}

		void close() override {
			_is_connected = false;
			int status = ::close(get_socket());

			if (status < 0) {
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

		callback begin_recv() override { return std::bind(&AbstractSocket::end_recv, this, false, 0); }
		callback begin_send() override { return std::bind(&AbstractSocket::end_send, this, false, 0);}
		void end_recv(bool, size_t) override {}
		void end_send(bool, size_t) override {}

		virtual ~AbstractSocket() { };
	};
}

#endif /* SRC_ABSTRACTSOCKET_H_ */
