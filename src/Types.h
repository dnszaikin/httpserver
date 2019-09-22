/*
 * Types.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <vector>

using byte_vector = std::vector<char>;

namespace network::web {
	struct HTTP {
		enum class Method {
			GET=1, POST=2, NOTIMPLEMENTED=-1
		};
	};
}

#endif /* SRC_TYPES_H_ */
