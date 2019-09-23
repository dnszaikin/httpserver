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
#include "WebSessionHandlerFactory.h"

using namespace std;

int main(int argc, char **argv) {

	try {
		using web = network::web::WebSession<network::UnixClientSocket>;

		network::AsyncUnixServer<web> server;

		auto&& wshandler = std::make_shared<network::web::WebSessionHandlerFactory>();

		server.set_handler_factory(wshandler);

		server.listen("8080");

		server.stop();
	} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	} catch (...) {
		cerr << "Unexpected program termination" << endl;
	}
	return 0;
}
