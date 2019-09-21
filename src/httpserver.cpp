/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#include <iostream>

#include "AsyncUnixServer.h"
#include <functional>
#include <future>

using namespace std;

class HTTPServer {
private:
public:
	HTTPServer();
	virtual ~HTTPServer();

};

int main(int argc, char **argv) {

	try {
		network::AsyncUnixServer server;

		std::thread(&network::AsyncUnixServer::listen, server, "8888").detach();

		while (true) {
//			auto&& clients = server.get_clients();
//			for (auto&& item : clients) {
//				auto&& client = item.second;
//				if (client->get_socket() != server.get_socket()) {
//					byte_vector bv;
//					client->swap_received(bv);
//					client->append_data_to_send(bv);
//				}
//			}
		}

		server.stop();
	} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	}
	return 0;
}
