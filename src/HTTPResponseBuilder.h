/*
 * HTTPBuilder.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_HTTPRESPONSEBUILDER_H_
#define SRC_HTTPRESPONSEBUILDER_H_

#include <string>
#include <unordered_map>

#include "CommonUtils.h"
#include "HTTPDefaultServerResponses.h"
#include "Types.h"

namespace network::web {

	using namespace utils::network;

	class HTTPResponseBuilder {
	public:

		HTTPResponseBuilder() {

		}

		static std::string build_http_response(uint16_t code, bool keepalive) {

			std::stringstream sbody;
			size_t size = 0;
			std::string response;
			switch (code) {
				case 200: sbody << OK200Data << std::endl; size = OK200Data_Len; response=OK200; break;
				case 404: sbody << NotFound404Data << std::endl; size = NotFound404Data_Len; response=NotFound404; break;
				case 400: sbody << BadRequest400Data << std::endl; size = BadRequest400Data_Len; response=BadRequest400; break;
				default: break;
			}

			std::stringstream sresponse;
			sresponse << Protocol << " " << response << std::endl;
			sresponse << "Date: " << get_http_date() << std::endl;
			sresponse << "Server: ExampleHttpd" << std::endl;
			sresponse << "Last-Modified: " << get_http_date() << std::endl;
			sresponse << "Content-Length: " << std::to_string(size) << std::endl;
			sresponse << "Content-Type: text/html" << std::endl;
			sresponse << "Connection: " << (keepalive ? "keep-alive" : "Close") << std::endl << std::endl;
			sresponse << sbody.str() << std::endl;

			return sresponse.str();
		}

		virtual ~HTTPResponseBuilder() {

		}
	};

}

#endif /* SRC_HTTPRESPONSEBUILDER_H_ */
