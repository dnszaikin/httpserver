/*
 * ISocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ISOCKET_H_
#define SRC_ISOCKET_H_

#include <string>

namespace network {
	class ISocket
	{
	public:
		virtual void init(std::string_view, std::string_view) = 0;
		virtual void init(std::string_view, std::string_view, int) = 0;
		virtual void shutdown() = 0;
		virtual void close() = 0;
		virtual ~ISocket() {};
	};
}


#endif /* SRC_ISOCKET_H_ */
