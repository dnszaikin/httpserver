/*
 * AbstractSocket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ABSTRACTSERVER_H_
#define SRC_ABSTRACTSERVER_H_

#include "IServer.h"

namespace network {
	class AbstractServer: public IServer {
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
		AbstractServer(): _listen(false), _port{} {};

		void listen(std::string_view) override {};

		/*
		 * close connection, but not free socket!
		 */
		void stop() override {};

		/*
		 * get IP Port
		 */
		std::string get_port() const {
			return _port;
		}

		bool is_listening() const {
			return _listen;
		}

		virtual ~AbstractServer() {};
	};
}

#endif /* SRC_ABSTRACTSERVER_H_ */
