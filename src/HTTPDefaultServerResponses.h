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
#include "CommonUtils.h"
#include "Types.h"

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

		static std::string build_http_response(uint16_t code, bool keepalive, const std::string& body, int16_t size = 0) {
			std::string response;
			switch (code) {
				case 200: response=OK200; break;
				case 404: response=NotFound404; break;
				case 400: response=BadRequest400; break;
				default: break;
			}

			std::stringstream sresponse;
			sresponse << Protocol << " " << response << std::endl;
			sresponse << "Date: " << get_http_date() << std::endl;
			sresponse << "Server: ExampleHttpd" << std::endl;
			sresponse << "Last-Modified: " << get_http_date() << std::endl;
			if (size != -1) {
				sresponse << "Content-Length: " << (size == 0 ? std::to_string(body.length()): std::to_string(size)) << std::endl;
			}
			sresponse << "Content-Type: text/html" << std::endl;
			sresponse << "Connection: " << (keepalive ? "keep-alive" : "Close") << std::endl << std::endl;
			sresponse << body << std::endl;

			return sresponse.str();
		}

		virtual ~HTTPResponseBuilder() {

		}
	};

	class DefaultServerResponses {
	private:
		static byte_vector _response404;
		static byte_vector _response200;
		static byte_vector _response400;
		static byte_vector _response404_keepalive;
		static byte_vector _response200_keepalive;
		static byte_vector _response400_keepalive;

	public:
		DefaultServerResponses() {
			std::string str(HTTPResponseBuilder::build_http_response(404, false));

			_response404.assign(str.begin(), str.end());

			str.assign(HTTPResponseBuilder::build_http_response(200, false));

			_response200.assign(str.begin(), str.end());

			str.assign(HTTPResponseBuilder::build_http_response(400, false));

			_response400.assign(str.begin(), str.end());

			str.assign(HTTPResponseBuilder::build_http_response(404, true));

			_response404_keepalive.assign(str.begin(), str.end());

			str.assign(HTTPResponseBuilder::build_http_response(200, true));

			_response200_keepalive.assign(str.begin(), str.end());

			str.assign(HTTPResponseBuilder::build_http_response(400, true));

			_response400_keepalive.assign(str.begin(), str.end());

		}

	private:
		static const byte_vector& _get_response_keepalive(uint16_t code)  {
			switch (code) {
				case 200: return _response200_keepalive; break;
				case 400: return _response400_keepalive; break;
				case 404: return _response404_keepalive; break;
				default: break;
			}
			throw std::runtime_error("Unimplemented response: " + std::to_string(code));
		}

		static const byte_vector& _get_response(uint16_t code)  {

			switch (code) {
				case 200: return _response200; break;
				case 400: return _response400; break;
				case 404: return _response404; break;
				default: break;
			}

			throw std::runtime_error("Unimplemented response: " + std::to_string(code));
		}

	public:
		static void get_response(int16_t code, bool keepalive, byte_vector& response)  {
			if (keepalive) {
				auto&& tmp = _get_response_keepalive(code);
				response.assign(tmp.begin(), tmp.end());
			} else {
				auto&& tmp = _get_response(code);
				response.assign(tmp.begin(), tmp.end());
			}
		}
	};

	byte_vector DefaultServerResponses::_response404;
	byte_vector DefaultServerResponses::_response200;
	byte_vector DefaultServerResponses::_response400;
	byte_vector DefaultServerResponses::_response404_keepalive;
	byte_vector DefaultServerResponses::_response200_keepalive;
	byte_vector DefaultServerResponses::_response400_keepalive;

	auto&& def = DefaultServerResponses();
}



#endif /* SRC_HTTPDEFAULTSERVERRESPONSES_H_ */
