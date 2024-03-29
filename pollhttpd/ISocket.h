/*
 * ISocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_ISOCKET_H_
#define POLLHTTPD_ISOCKET_H_

#include <string>
#include <memory>
#include <functional>

#include "IHandlerFactory.h"
#include "Types.h"

#ifndef _WIN32
#define socket_t int
#else
#define socket_t SOCKET
#endif
namespace dnszaikin::pollhttpd::network {
	typedef std::function<void()> callback;

	class ISocket
	{
	protected:
		virtual void build_name() =0;
		IHandlerFactory::ptr _handler;
	public:
		typedef std::shared_ptr<ISocket> ptr;
		virtual void init(std::string_view, std::string_view) = 0;
		virtual void init(std::string_view, std::string_view, socket_t, IHandlerFactory::ptr) = 0;
		virtual void shutdown() = 0;
		virtual void close() = 0;
		virtual std::string_view get_name() const = 0;
		virtual std::string_view get_port() const = 0;
		virtual std::string_view get_host() const = 0;
		virtual socket_t get_socket() const = 0;
		virtual callback begin_recv() = 0;
		virtual callback begin_send() = 0;
		virtual void end_recv(bool, size_t) = 0;
		virtual void end_send(bool, size_t) = 0;
		virtual bool is_connected() const = 0;
		virtual void swap_received(byte_vector&) = 0;
		virtual void append_data_to_send(const byte_vector&) = 0;
		virtual ~ISocket() {};
	};
}


#endif /* POLLHTTPD_ISOCKET_H_ */
