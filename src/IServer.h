/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ISERVER_H_
#define SRC_ISERVER_H_

#include <string>

namespace network {
	class IServer
	{
	public:

		virtual void listen(std::string_view port) = 0;
		virtual void stop() = 0;
		virtual int get_socket() = 0;
		virtual ~IServer() {};
	};
}


#endif /* SRC_ISERVER_H_ */
