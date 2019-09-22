/*
 * IHandler.h
 *
 *  Created on: Sep 23, 2019
 *      Author: user
 */

#ifndef SRC_IHANDLER_H_
#define SRC_IHANDLER_H_

#include "Types.h"

namespace network {
	class IHandler {
	public:

		virtual void handler(byte_vector& request, byte_vector& response)= 0;
		virtual ~IHandler() {};
	};
}



#endif /* SRC_IHANDLER_H_ */
