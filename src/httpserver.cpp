/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#include <iostream>

#include "AsyncUnixServer.h"

using namespace std;

class HTTPServer {
private:
public:
	HTTPServer();
	virtual ~HTTPServer();

};

int main(int argc, char **argv) {

	try {
		network::AsyncUnixServer socket;

		socket.listen("8888");
		socket.stop();
	} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	}
	return 0;
}
