/*
 * WebSessionManager.h
 *
 *  Created on: Sep 22, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_WEBSESSIONHANDLERFACTORY_H_
#define SRC_WEBSESSIONHANDLERFACTORY_H_

#include <memory>
#include <unordered_map>
#include <mutex>

#include "IHandlerFactory.h"
#include "Types.h"
#include "HTTPDefaultServerResponses.h"
#include "IRequestHandler.h"
#include "PollingHelper.h"

namespace network::web {

	class Storage {
	public:
		typedef std::unordered_map<std::string, size_t> data_map_t;
	private:
		data_map_t _count_commands;
		std::mutex _count_commands_mutex;
	public:
		Storage() {}

		virtual ~Storage() {}

		void count(const std::string& cmd) {
			std::unique_lock<std::mutex> lock(_count_commands_mutex);

			auto it = _count_commands.find(cmd);
			if (it != _count_commands.end()) {
				++it->second;
			} else {
				_count_commands.emplace(cmd, 1);
			}
		}

		void swap(data_map_t& map) {
			std::unique_lock<std::mutex> lock(_count_commands_mutex);
			_count_commands.swap(map);
		}
	};

	class CommandServer : IRequestHandler {
	private:
		std::shared_ptr<Storage> _storage;
	public:
		CommandServer() {}

		virtual ~CommandServer() {}

		void set_data_source(std::shared_ptr<Storage> storage) {
			_storage = storage;
		}

		void handle_request(const HTTPRequestParser& request, byte_vector& response, int socket) override {
			auto vec = utils::common::split(request.get_url(), '/');

			if (vec.size() != 3) {
				DefaultServerResponses::get_response(400, false, response);
				LOG_ERROR("Bad url: " << request.get_url());
			} else {
				std::string cmd = vec.at(2);
				_storage->count(cmd);
				DefaultServerResponses::get_response(200, false, response);
			}
		}
	};

	class StatisticServer : IRequestHandler {
	private:
		std::shared_ptr<Storage> _storage;
		Storage::data_map_t _map;
		std::thread _thread;
		bool _thread_started;
		std::stringstream _output_stream;
		int _socket;
	public:
		StatisticServer(): _thread_started(false), _socket(-1) {
			_thread = std::thread(&StatisticServer::run, this);
		};

		void set_data_source(std::shared_ptr<Storage> storage) {
			_storage = storage;
		}

		void run() {
			_thread_started = true;
			while (_thread_started) {
				_map.clear();
				_storage->swap(_map);

				_output_stream.clear();
				_output_stream.str("");
				for (auto&& item: _map) {
					_output_stream << item.first << ": " << item.second << std::endl;
				}

				sleep(10);


//				if (_socket->is_connected()) {
//					std::string answer = _output_stream.str();
//					if (answer.empty()) {
//						answer = "No data";
//					}
//
//					byte_vector bv;
//
//					auto&& tmp = HTTPResponseBuilder::build_http_response(200, true, answer);
//
//					bv.assign(tmp.begin(), tmp.end());
//					_socket->append_data_to_send(bv);
//					auto res = _socket->begin_send();
//					res();
//				}
			}
		}

		void handle_request(const HTTPRequestParser& request, byte_vector& response, int socket) override {
			std::string answer = _output_stream.str();
			if (answer.empty()) {
				answer = "No data";
			}
			auto&& tmp = HTTPResponseBuilder::build_http_response(200, true, answer);
			response.assign(tmp.begin(), tmp.end());
		}

		virtual ~StatisticServer() {
			_thread_started = false;

			if (_thread.joinable()) {
				_thread.join();
			}
		}
	};

	class WebSessionHandlerFactory : public IHandlerFactory, public std::enable_shared_from_this<WebSessionHandlerFactory> {
	private:
		HTTPRequestParser _http_request_parser;
		StatisticServer _statistic_srv;
		CommandServer _command_srv;

	public:
		WebSessionHandlerFactory() {
			auto storage = std::make_shared<Storage>();
			_statistic_srv.set_data_source(storage);
			_command_srv.set_data_source(storage);
		}

		void handler(byte_vector& request, byte_vector& response, bool& keepalive, int socket) override {
			_http_request_parser.parse_http(request.cbegin(), request.cend());
			keepalive = _http_request_parser.get_keepalive();

			LOG_DEBUG("Method: " << _http_request_parser.get_method_str() << ", url: " << _http_request_parser.get_url()
					<< ", protocol: " << _http_request_parser.get_protocol() << ", keep-alive: " << keepalive);

			auto vec = utils::common::split(_http_request_parser.get_url(), '/');

			if (!vec.empty()) {
				auto cmd = vec.at(1);
				if (cmd == "command") {
					_command_srv.handle_request(_http_request_parser, response, socket);
				} else if (cmd == "statistics") {
					_statistic_srv.handle_request(_http_request_parser, response, socket);
				} else {
					keepalive = false;
					DefaultServerResponses::get_response(404, false, response);
				}
			} else {
				keepalive = false;
				DefaultServerResponses::get_response(400, false, response);
			}
		}

		virtual ~WebSessionHandlerFactory() {
		}
	};
}




#endif /* SRC_WEBSESSIONHANDLERFACTORY_H_ */
