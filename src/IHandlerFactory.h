/*
 * IHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_IHANDLERFACTORY_H_
#define SRC_IHANDLERFACTORY_H_

#include <memory>
#include "Types.h"
#include "IRequestHandler.h"

namespace network {

	class IHandlerFactory {
	public:
		typedef std::shared_ptr<IHandlerFactory> ptr;
		virtual web::IRequestHandler::ptr handler(byte_vector& request, byte_vector& response, bool& keepalive)= 0;
		virtual ~IHandlerFactory() {};
	};
}



#endif /* SRC_IHANDLERFACTORY_H_ */
