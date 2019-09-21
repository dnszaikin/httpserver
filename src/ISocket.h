/*
 * ISocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_ISOCKET_H_
#define SRC_ISOCKET_H_

#include <string>
#include <memory>

namespace network {
	class ISocket
	{
	public:
		typedef std::shared_ptr<ISocket> ptr;
		virtual void init(std::string_view, std::string_view) = 0;
		virtual void init(std::string_view, std::string_view, int) = 0;
		virtual void shutdown() = 0;
		virtual void close() = 0;
		virtual std::string_view get_port() const = 0;
		virtual std::string_view get_host() const = 0;
		virtual int get_socket() const = 0;
		virtual void recv() = 0;
		virtual void send() = 0;
		virtual bool is_connected() const = 0;
		virtual ~ISocket() {};
	};
}


#endif /* SRC_ISOCKET_H_ */
