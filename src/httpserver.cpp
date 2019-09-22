/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#include <iostream>
#include <functional>
#include <future>

#include "AsyncUnixServer.h"
#include "WebSession.h"

using namespace std;

class HTTPServer {
private:
public:
	HTTPServer();
	virtual ~HTTPServer();

};

int main(int argc, char **argv) {

	try {
		using web = network::web::WebSession<network::UnixClientSocket>;

		network::AsyncUnixServer<web> server;

//		std::thread(&network::AsyncUnixServer<web>::listen, std::ref(server), "8888").detach();

		//waiting while server initialization is complete
		server.listen("8080");

//		while (!server.is_listening()) { std::this_thread::yield(); };

//		while (server.is_listening()) {
////			auto&& clients = server.get_clients();
////			//LOG_INFO("Clients: " << clients.size());
////			for (auto&& item : clients) {
////				auto&& client = item.second;
////				if (client->get_socket() != server.get_socket()) {
////					byte_vector bv;
////					client->swap_received(bv);
////					LOG_INFO(client->get_name() << ": Received " << bv.size() << " bytes" );
////					client->append_data_to_send(bv);
////				}
////			}
//			std::this_thread::yield();
//		}

		server.stop();
	} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	}
	return 0;
}
