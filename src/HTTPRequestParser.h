/*
 * HTTP.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_HTTPREQUESTPARSER_H_
#define SRC_HTTPREQUESTPARSER_H_

#include <string>
#include <unordered_map>

#include "CommonUtils.h"
#include "Types.h"

namespace network::web {

	class HTTPRequestParser {
	private:
		std::string _data;
		std::string _url;
		std::string _protocol;
		std::string _method_str;
		bool _keepalive;
		HTTP::Method _method;
		std::unordered_map<std::string, std::string> _params;

	public:

		HTTPRequestParser(): _data{}, _url{}, _protocol{}, _method_str{}, _keepalive(false), _method(HTTP::Method::NOTIMPLEMENTED) {

		}

		void parse_http(byte_vector::const_iterator begin, byte_vector::const_iterator end)  {
			_data.assign(begin, end);

			std::istringstream resp(_data);
			std::string header;
			size_t index;
			size_t line = 0;

			while (std::getline(resp, header) && header != "\r") {
				if (line == 0) {
					parse_header(header);
				} else {
					index = header.find(':', 0);
					if(index != std::string::npos) {
						_params.emplace(utils::common::trim_copy(header.substr(0, index)),
									utils::common::trim_copy(header.substr(index + 1)));
					}
				}
				++line;
			}

			auto it = _params.find("Connection");

			if (it != _params.end()) {
				if (it->second == "keep-alive") {
					_keepalive = true;
				}
			}
		}

		void parse_header(const std::string& header) {

			auto&& vec = utils::common::split(header, ' ');
			if (vec.size() != 3) {
				throw std::runtime_error("Unable to parse HTTP header: " + header);
			} else {
				_method_str = vec.at(0);
				_url = vec.at(1);
				_protocol = vec.at(2);
			}

			size_t it;
			if ((it = _method_str.find("GET")) != std::string::npos) {
				_method = HTTP::Method::GET;
			} else if ((it = _method_str.find("POST")) != std::string::npos) {
				_method = HTTP::Method::POST;
			} else {
				_method = HTTP::Method::NOTIMPLEMENTED;
			}
		}

		HTTP::Method get_method() {
			return _method;
		}

		std::string_view get_method_str() {
			return _method_str;
		}

		std::string_view get_url() {
			return _url;
		}

		std::string_view get_protocol() {
			return _protocol;
		}

		bool get_keepalive() {
			return _keepalive;
		}

		virtual ~HTTPRequestParser() {

		}
	};

}

#endif /* SRC_HTTPREQUESTPARSER_H_ */
