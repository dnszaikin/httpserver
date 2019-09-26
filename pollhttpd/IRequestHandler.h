/*
 * IRequestHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_IREQUESTHANDLER_H_
#define POLLHTTPD_IREQUESTHANDLER_H_

#include "HTTPRequestParser.h"
#include "Types.h"

namespace dnszaikin::pollhttpd::network::web {

	class IRequestHandler {
	public:
		typedef std::shared_ptr<IRequestHandler> ptr;
		virtual void handle_request(const HTTPRequestParser& request, byte_vector& response) = 0;
		virtual bool is_keepalive() = 0;
		virtual void get_data(byte_vector& bv) = 0;
		virtual void shutdown() = 0;
		virtual ~IRequestHandler() {};
	};
}



#endif /* POLLHTTPD_IREQUESTHANDLER_H_ */
