/*
 * Types.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef POLLHTTPD_TYPES_H_
#define POLLHTTPD_TYPES_H_

#include <vector>

using byte_vector = std::vector<char>;

namespace dnszaikin::pollhttpd::network::web {
	struct HTTP {
		enum class Method {
			GET=1, POST=2, NOTIMPLEMENTED=-1
		};
	};
}

#endif /* POLLHTTPD_TYPES_H_ */
