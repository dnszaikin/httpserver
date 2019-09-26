/*
 * socket.h
 *
 *  Created on: Sep 25, 2019
 *      Author: dnszaikin
 */

#include <iostream>
#include <functional>
#include <future>

#include "../pollhttpd/WebSession.h"
#include "WebSessionHandlerFactory.h"

#ifndef _WIN32
#include "../pollhttpd/AsyncUnixServer.h"
#else
#include "AsyncWindowsServer.h"
#endif

using namespace std;

int main(int argc, char **argv) {

	try {
#ifndef _WIN32
		using web = dnszaikin::pollhttpd::network::web::WebSession<dnszaikin::pollhttpd::network::UnixClientSocket>;

		dnszaikin::pollhttpd::network::AsyncUnixServer<web> server;

		auto&& wshandler = std::make_shared<examples::WebSessionHandlerFactory>();

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

		using web = dnszaikin::pollhttpd::network::web::WebSession<network::WindowsClientSocket>;

		dnszaikin::pollhttpd::network::AsyncWindowsServer<web> server;

		auto&& wshandler = std::make_shared<dnszaikin::pollhttpd::network::web::WebSessionHandlerFactory>();

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
