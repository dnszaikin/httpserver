/*
 * WebSession.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_WEBSESSION_H_
#define SRC_WEBSESSION_H_

#include "HTTPRequestParser.h"

namespace network::web {

	template <class T>
	class WebSession: public T, std::enable_shared_from_this<WebSession<T>> {
	private:
		bool _keepalive;
		std::stringstream _output;

	public:
		WebSession() : T(), _keepalive(false) {

		}

		void set_keepalive() {
			_keepalive = true;
		}

		void send(const byte_vector& data) {
			T::append_data_to_send(data);
			auto send_res =T::begin_send();
			send_res();
		}

		void end_recv(bool complete, size_t size) override {
			T::end_recv(complete, size);
			if (complete) {
				byte_vector bv;
				T::swap_received(bv);

				byte_vector response;

				this->_handler->handler(bv, response, _keepalive, this->get_socket());
				send(response);
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
