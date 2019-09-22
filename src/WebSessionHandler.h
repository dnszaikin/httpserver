/*
 * WebSessionManager.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_WEBSESSIONHANDLER_H_
#define SRC_WEBSESSIONHANDLER_H_

#include <memory>

#include "Types.h"
#include "IHandler.h"

namespace network::web {

	class WebSessionHandler : public IHandler {
	private:
		byte_vector _response400;
		HTTPRequestParser _http_request_parser;
		bool _keepalive;
	public:
		WebSessionHandler() {
			std::string str(HTTPResponseBuilder::build_http_response(400, false));
			_response400.assign(str.begin(), str.end());
		}

		void handler(byte_vector& request, byte_vector& response) override {
			_http_request_parser.parse_http(request.cbegin(), request.cend());
			_keepalive = _http_request_parser.get_keepalive();

			LOG_DEBUG("Method: " << _http_request_parser.get_method_str() << ", url: " << _http_request_parser.get_url()
					<< ", protocol: " << _http_request_parser.get_protocol() << ", keep-alive: " << _keepalive);
			response.assign(_response400.begin(), _response400.end());
		}

		virtual ~WebSessionHandler() {
		}
	};
}




#endif /* SRC_WEBSESSIONHANDLER_H_ */
