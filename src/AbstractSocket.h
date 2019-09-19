/*
 * AbstractSocket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ABSTRACTSOCKET_H_
#define SRC_ABSTRACTSOCKET_H_

#include "ISocket.h"

namespace network {
	class AbstractSocket: public ISocket {
	private:
		bool _listen;
		std::string _port;
	protected:

		void set_listen () {
			_listen = true;
		}

		void set_port(std::string_view port) {
			_port = port;
		}

	public:
		AbstractSocket(): _listen(false), _port{} {};

		void listen(std::string_view) override {};

		/*
		 * close connection, but not free socket!
		 */
		void close() override {};

		/*
		 * get IP Port
		 */
		std::string get_port() const {
			return _port;
		}

		bool is_listening() const {
			return _listen;
		}

		virtual ~AbstractSocket() {};
	};
}

#endif /* SRC_ABSTRACTSOCKET_H_ */
