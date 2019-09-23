/*
 * IRequestHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_IREQUESTHANDLER_H_
#define SRC_IREQUESTHANDLER_H_

#include "HTTPRequestParser.h"
#include "Types.h"

namespace network::web {

	class IRequestHandler {
	public:
		virtual void handle_request(const HTTPRequestParser& request, byte_vector& response, int) = 0;
		virtual ~IRequestHandler() {};
	};
}



#endif /* SRC_IREQUESTHANDLER_H_ */
