/*
 * HTTP.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_HTTP_H_
#define SRC_HTTP_H_

#include <string>
#include <unordered_map>

#include "CommonUtils.h"
#include "Types.h"

namespace network::web {

	class HTTP {
	public:
		enum class Method {
			GET=1, POST=2, NOTIMPLEMENTED=-1
		};

	private:
		std::string _data;
		std::string _url;
		std::string _protocol;
		Method _method;
		std::string _method_str;
		std::unordered_map<std::string, std::string> _params;

	public:

		HTTP(): _data{}, _url{}, _protocol{}, _method(Method::NOTIMPLEMENTED) {

		}

		void parse_HTTP(byte_vector::const_iterator begin, byte_vector::const_iterator end)  {
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
				_method = Method::GET;
			} else if ((it = _method_str.find("POST")) != std::string::npos) {
				_method = Method::POST;
			} else {
				_method = Method::NOTIMPLEMENTED;
			}

		}

		Method get_method() {
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

		virtual ~HTTP() {

		}
	};

}



#endif /* SRC_HTTP_H_ */
