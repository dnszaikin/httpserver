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
		web::IRequestHandler::ptr _ptr;
		std::thread _thread;
		bool _thread_run;
	public:
		WebSession() : T(), _keepalive(false), _thread_run(false) {

		}

		void set_keepalive() {
			_keepalive = true;
		}

		void run() {
			byte_vector response;
			_thread_run = true;
			while (this->is_connected() && _thread_run) {
				_ptr->get_data(response);

				if (!response.empty()) {
					send(response);
				}

				std::this_thread::yield();
			}
			_thread_run = false;
		}

		void send(const byte_vector& data) {
			T::append_data_to_send(data);
			auto send_res =T::begin_send();
			send_res();
		}

		void end_recv(bool complete, size_t size) override {
			T::end_recv(complete, size);

			if (complete) {

				if (size == 0) {
					return;
				}

				byte_vector bv;
				T::swap_received(bv);

				byte_vector response;

				_ptr = this->_handler->handler(bv, response, _keepalive);

				send(response);

				if (_ptr) {
					if (_ptr->is_keepalive()) {
						std::thread(&WebSession::run, this).detach();
					}
				}
			}
		}

		void end_send(bool complete, size_t size) override {
			T::end_send(complete, size);

//			if (_keepalive || (_ptr && _ptr->is_keepalive())) {
//				return;
//			}
//
//			if (complete) {
//				T::shutdown();
//				T::close();
//			}
		}

		virtual ~WebSession() {
			LOG_INFO(this->get_name() << " web session destroying...");
			_thread_run = false;

			if (_ptr) {
				_ptr->shutdown();
			}
//			if (_thread.joinable()) {
////				T::shutdown();
////				T::close();
//				_thread.join();
//			}
		}
	};
}



#endif /* SRC_WEBSESSION_H_ */
