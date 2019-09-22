/*
 * HTTPResponses.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_HTTPRESPONSES_H_
#define SRC_HTTPRESPONSES_H_

#include <string>
#include <cstring>

namespace network::web {

	constexpr char NotFound404[]  {
		"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"
		"<html>"
		"<head>"
			"<title>404 Not Found</title>"
		"</head>"
		"<body>"
		   "<h1>Not Found</h1>"
		   "<p>The requested URL /t.html was not found on this server.</p>"
		"</body>"
		"</html>"
	};

	constexpr size_t NotFound404_Len = std::strlen(NotFound404);

	constexpr char OK200[] {
		"<html>"
			"<body>"
				"<h1>OK</h1>"
			"</body>"
		"</html>"
	};

	constexpr size_t OK200_Len = std::strlen(OK200);
}



#endif /* SRC_HTTPRESPONSES_H_ */
