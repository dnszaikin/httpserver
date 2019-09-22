/*
 * WebSession.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_WEBSESSION_H_
#define SRC_WEBSESSION_H_

#include "HTTPResponses.h"
#include "HTTP.h"

namespace network::web {

	template <class T>
	class WebSession: public T {
	private:
		bool _keepalive;
		HTTP _http;

		void get_request_type() {

		}

		std::string build_http_response(uint16_t code) {

			std::stringstream sbody;
			size_t size = 0;

			switch (code) {
				case 200: sbody << OK200 << std::endl; size = OK200_Len; break;
				case 404: sbody << NotFound404 << std::endl; size = NotFound404_Len; break;
				default: break;
			}

			std::stringstream sresponse;
			sresponse << "HTTP/1.1 " << std::to_string(code) << " OK" << std::endl;
			sresponse << "Date: " << get_http_date() << std::endl;
			sresponse << "Server: ExampleHttpd" << std::endl;
			sresponse << "Last-Modified: " << get_http_date() << std::endl;
			sresponse << "Content-Length: " << std::to_string(size) << std::endl;
			sresponse << "Content-Type: text/html" << std::endl;
			sresponse << "Connection: Close" << std::endl << std::endl;
			sresponse << sbody.str() << std::endl;

			return sresponse.str();
		}

	public:
		WebSession() : T(), _keepalive(false) {

		}

		void set_keepalive() {
			_keepalive = true;
		}

		void end_recv(bool complete, size_t size) override {
			T::end_recv(complete, size);
			if (complete) {
				byte_vector bv;
				T::swap_received(bv);

				_http.parse_HTTP(bv.cbegin(), bv.cend());

				LOG_DEBUG("Method: " << _http.get_method_str() << ", url: " << _http.get_url()
						<< ", protocol: " << _http.get_protocol());

				std::string str(build_http_response(200));

				bv.assign(str.begin(), str.end());

				T::append_data_to_send(bv);
				auto send_res =T::begin_send();
				send_res();
			}
		}

		void end_send(bool complete, size_t size) override {
			T::end_send(complete, size);

			if (complete && _keepalive) {
				T::shutdown();
				T::close();
			}
		}

		virtual ~WebSession() {

		}
	};
}



#endif /* SRC_WEBSESSION_H_ */
