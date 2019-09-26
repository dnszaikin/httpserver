/*
 * IHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_IHANDLERFACTORY_H_
#define POLLHTTPD_IHANDLERFACTORY_H_

#include <memory>

#include "IRequestHandler.h"
#include "Types.h"

namespace dnszaikin::pollhttpd::network {

	class IHandlerFactory {
	public:
		typedef std::shared_ptr<IHandlerFactory> ptr;
		virtual web::IRequestHandler::ptr handler(byte_vector& request, byte_vector& response, bool& keepalive)= 0;
		virtual ~IHandlerFactory() {};
	};
}



#endif /* POLLHTTPD_IHANDLERFACTORY_H_ */
