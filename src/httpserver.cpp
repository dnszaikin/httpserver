/*
 * socket.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#include <iostream>
#include "UnixSocket.h"

using namespace std;

class HTTPServer {
private:
public:
	HTTPServer();
	virtual ~HTTPServer();

};

int main(int argc, char **argv) {

	try {
		network::UnixSocket socket;

		socket.listen("8888");
		socket.close();
	} catch (const std::exception& e) {
		cerr << "Error: " <<  e.what() << endl;
	}
	return 0;
}
