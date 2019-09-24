/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#include <iostream>
#include <functional>
#include <future>

#include "WebSession.h"
#include "WebSessionHandlerFactory.h"

#ifndef _WIN32
#include "AsyncUnixServer.h"
#else
#include "AsyncWindowsServer.h"
#endif

using namespace std;

int main(int argc, char **argv) {

	try {
#ifndef _WIN32
		using web = network::web::WebSession<network::UnixClientSocket>;

		network::AsyncUnixServer<web> server;

		auto&& wshandler = std::make_shared<network::web::WebSessionHandlerFactory>();

		server.set_handler_factory(wshandler);

		server.listen("8080");

		server.stop();
#else
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			throw std::runtime_error("Cant find winsock32.dll");
		}

		using web = network::web::WebSession<network::WindowsClientSocket>;

		network::AsyncWindowsServer<web> server;

		auto&& wshandler = std::make_shared<network::web::WebSessionHandlerFactory>();

		server.set_handler_factory(wshandler);

		server.listen("8080");

		server.stop();

		WSACleanup();

#endif // !_WIN32

			} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	} catch (...) {
		cerr << "Unexpected program termination" << endl;
	}
	return 0;
}
