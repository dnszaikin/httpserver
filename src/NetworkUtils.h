/*
 * NetworkUtils.h
 *
 *  Created on: Sep 20, 2019
 *      Author: user
 */

#ifndef SRC_NETWORKUTILS_H_
#define SRC_NETWORKUTILS_H_

#include <string>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#include "Logger.h"

namespace network {

#ifndef _WIN32
	inline std::string strerr() {
		return std::string(strerror(errno));
	}

	/*
	 * converts sockaddress to host and port representation. returns pair first is host, last is port
	 */
	inline std::pair<std::string_view, std::string_view> get_name_info(const struct sockaddr& address) {

		char hostname[128];
		char port[128];
		if(::getnameinfo(&address, sizeof(sockaddr), hostname, sizeof(hostname),
				port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) != 0){
			LOG_ERROR("Failed to translate socket to client address. Error: " + strerr());
		}

		return std::make_pair<std::string_view, std::string_view>(hostname, port);
	}
#endif

}



#endif /* SRC_NETWORKUTILS_H_ */
