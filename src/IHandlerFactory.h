/*
 * IHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: user
 */

#ifndef SRC_IHANDLERFACTORY_H_
#define SRC_IHANDLERFACTORY_H_

#include "Types.h"

namespace network {
	class IHandlerFactory {
	public:

		virtual void handler(byte_vector& request, byte_vector& response)= 0;
		virtual ~IHandlerFactory() {};
	};
}



#endif /* SRC_IHANDLERFACTORY_H_ */
