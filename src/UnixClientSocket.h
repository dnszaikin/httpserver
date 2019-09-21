/*
 * UnixClientSocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_UNIXCLIENTSOCKET_H_
#define SRC_UNIXCLIENTSOCKET_H_

#include "AbstractSocket.h"

namespace network {
class UnixClientSocket: public network::AbstractSocket {
public:
	UnixClientSocket(): AbstractSocket() {} ;

	UnixClientSocket(const UnixClientSocket&): AbstractSocket() {} ;

	void init(std::string_view host, std::string_view port, int socket_fd) override {
		set_host(host);
		set_port(port);
		set_socket(socket_fd);

		int opt = 1;
		ioctl(get_socket(), FIONBIO, &opt);
	}

	virtual ~UnixClientSocket() {};

};

}

#endif /* SRC_UNIXCLIENTSOCKET_H_ */
#endif
