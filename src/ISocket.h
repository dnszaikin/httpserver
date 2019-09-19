/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ISOCKET_H_
#define SRC_ISOCKET_H_

#include <string>

namespace network {
	class ISocket
	{
	public:

		virtual void listen(std::string_view port) = 0;
		virtual void close() = 0;
		virtual ~ISocket() {};
	};
}


#endif /* SRC_ISOCKET_H_ */
