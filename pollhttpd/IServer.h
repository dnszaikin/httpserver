/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_ISERVER_H_
#define POLLHTTPD_ISERVER_H_

#include <string>

namespace dnszaikin::pollhttpd::network {
	class IServer
	{
	public:

		virtual void listen(std::string_view port) = 0;
		virtual void stop() = 0;
		virtual int get_socket() = 0;
		virtual ~IServer() {};
	};
}


#endif /* POLLHTTPD_ISERVER_H_ */
