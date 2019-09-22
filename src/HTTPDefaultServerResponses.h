/*
 * HTTPResponses.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_HTTPDEFAULTSERVERRESPONSES_H_
#define SRC_HTTPDEFAULTSERVERRESPONSES_H_

#include <string>
#include <cstring>

namespace network::web {

	constexpr char NotFound404[] {"404 Not Found"};

	constexpr char NotFound404Data[]  {
		"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
		"<html>"
		"<head>"
			"<title>404 Not Found</title>"
		"</head>"
		"<body>"
		   "<h1>Not Found</h1>"
		"</body>"
		"</html>"
	};

	constexpr size_t NotFound404Data_Len = std::strlen(NotFound404Data);

	constexpr char OK200[] {"200 OK"};

	constexpr char OK200Data[] {
		"<html>"
			"<body>"
				"<h1>OK</h1>"
			"</body>"
		"</html>"
	};

	constexpr size_t OK200Data_Len = std::strlen(OK200Data);

	constexpr char BadRequest400[] {"400 Bad Request"};

	constexpr char BadRequest400Data[] {
		"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
		"<html>"
		"<head>"
			"<title>400 Bad Request</title>"
		"</head>"
		"<body>"
		   "<h1>Bad Request</h1>"
		"</body>"
		"</html>"
	};

	constexpr size_t BadRequest400Data_Len = std::strlen(BadRequest400Data);

	constexpr char Protocol[] {"HTTP/1.0"};
}



#endif /* SRC_HTTPDEFAULTSERVERRESPONSES_H_ */
