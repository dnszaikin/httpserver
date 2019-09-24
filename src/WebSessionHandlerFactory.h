/*
 * WebSessionHandlerFactory.h
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

	constexpr int TIMEOUT = 60;

	class Storage {
	public:
		typedef std::unordered_map<std::string, size_t> data_map_t;
	private:
		data_map_t _count_commands;
		data_map_t _count_commands_last;
		std::mutex _count_commands_mutex;
		std::thread _thread;
		bool _thread_run;
		std::condition_variable _thread_sleep;
		std::condition_variable _user_wait;
		std::mutex _user_wait_mutex;
		std::mutex _thread_sleep_mutex;
	public:
		typedef std::shared_ptr<Storage> ptr;

		Storage(): _thread_run(false) {
			_thread = std::thread(&Storage::run, this);
		}

		void run() {
			_thread_run = true;

			while (_thread_run) {
				{
					std::unique_lock<std::mutex> lock(_thread_sleep_mutex);
					_thread_sleep.wait_for(lock, std::chrono::seconds(TIMEOUT));
				}

//				_user_wait.notify_one();

				{
					std::unique_lock<std::mutex> lock(_count_commands_mutex);
					_count_commands_last.swap(_count_commands);
					_count_commands.clear();
				}
			}

			_thread_run = false;
		}

//		void wait() {
//			std::unique_lock<std::mutex> lock(_user_wait_mutex);
//			_user_wait.wait(lock);
//		}

		virtual ~Storage() {
			_thread_run = false;
			_thread_sleep.notify_one();
			if(_thread.joinable()) {
				_thread.join();
			}
		}

		void count(const std::string& cmd) {
			std::unique_lock<std::mutex> lock(_count_commands_mutex);

			auto it = _count_commands.find(cmd);
			if (it != _count_commands.end()) {
				++it->second;
			} else {
				_count_commands.emplace(cmd, 1);
			}
		}

		void get_data(data_map_t& map) {
			map = _count_commands_last;
		}
	};

	class CommandServer : public IRequestHandler {
	private:
		Storage::ptr _storage;
	public:
		CommandServer()  {}

		virtual ~CommandServer() {}

		void set_data_source(std::shared_ptr<Storage> storage) {
			_storage = storage;
		}

		void shutdown() override {

		}

		bool is_keepalive() override {
			return false;
		}

		void handle_request(const HTTPRequestParser& request, byte_vector& response) override {
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

		void get_data(byte_vector& data) override {}
	};

	class StatisticServer : public IRequestHandler {
	private:
		Storage::ptr _storage;
		Storage::data_map_t _map;
		std::thread _thread;
		bool _thread_started;
		std::stringstream _output_stream;
		std::mutex _answer_mutex;
		std::string _answer;
		bool _data_ready_notified;
	    std::condition_variable _data_ready_monitor;
	    std::condition_variable _thread_sleep;
	    std::mutex _thread_sleep_mutex;
	public:
		StatisticServer(): _thread_started(false), _answer(""), _data_ready_notified(false) {
			LOG_INFO("Statistics session started");
		};

		bool is_keepalive() override {
			return true;
		}

		void shutdown() override {
			LOG_INFO("Statistics session closed");
			_thread_started = false;
			_thread_sleep.notify_one();
			if (_thread.joinable()) {
				_thread.join();
			}
		}

		void set_data_source(std::shared_ptr<Storage> storage) {
			_storage = storage;
		}

		void get_data(byte_vector& data) override {
			std::unique_lock<std::mutex> lock(_answer_mutex);
//			while (!_data_ready_notified) {
				_data_ready_monitor.wait(lock);
//			}

			data.assign(_answer.begin(), _answer.end());
			_answer.clear();
		}

		void get_last_data() {
			_map.clear();
			_storage->get_data(_map);

			_output_stream.clear();
			_output_stream.str("");

			_output_stream << "Statistics for last " << TIMEOUT << " second(s) <br/>"  << std::endl;

			for (auto&& item: _map) {
				_output_stream << item.first << ": " << item.second << "<br/>" << std::endl;
			}
			_output_stream << "<br/>" << std::endl;
			_output_stream.flush();

			{
				std::unique_lock<std::mutex> lock(_answer_mutex);
				_answer = _output_stream.str();
			}
		}

		void run() {
			_thread_started = true;

			while (_thread_started) {

				get_last_data();

				_data_ready_monitor.notify_one();

				{
					std::unique_lock<std::mutex> lock(_thread_sleep_mutex);
					_thread_sleep.wait_for(lock, std::chrono::seconds(TIMEOUT));
				}

			}

			LOG_DEBUG("Statistic thread finished");
			_data_ready_monitor.notify_one();
			_thread_started = false;
		}

		void handle_request(const HTTPRequestParser& request, byte_vector& response) override {

			get_last_data();

			std::string tmp;

			{
				std::unique_lock<std::mutex> lock(_answer_mutex);
				if (_answer.empty()) {
					_answer = "No data<br/>";
				}
				tmp = HTTPResponseBuilder::build_http_response(200, true, _answer, -1);
			}

			response.assign(tmp.begin(), tmp.end());

			if (!_thread_started) {
				_thread = std::thread(&StatisticServer::run, this);
			}
		}

		virtual ~StatisticServer() {
			shutdown();
		}
	};

	class WebSessionHandlerFactory : public IHandlerFactory, public std::enable_shared_from_this<WebSessionHandlerFactory> {
	private:
		HTTPRequestParser _http_request_parser;
		Storage::ptr _storage;

	public:
		WebSessionHandlerFactory() {
			_storage = std::make_shared<Storage>();
		}

		web::IRequestHandler::ptr handler(byte_vector& request, byte_vector& response, bool& keepalive) override {
			_http_request_parser.parse_http(request.cbegin(), request.cend());
			keepalive = _http_request_parser.get_keepalive();

			LOG_DEBUG("Method: " << _http_request_parser.get_method_str() << ", url: " << _http_request_parser.get_url()
					<< ", protocol: " << _http_request_parser.get_protocol() << ", keep-alive: " << keepalive);

			auto vec = utils::common::split(_http_request_parser.get_url(), '/');

			if (!vec.empty()) {
				auto cmd = vec.at(1);
				if (cmd == "command") {
					auto ptr = std::make_shared<CommandServer>();
					ptr->set_data_source(_storage);
					ptr->handle_request(_http_request_parser, response);
					return ptr;
				} else if (cmd == "statistics") {
					auto ptr = std::make_shared<StatisticServer>();
					ptr->set_data_source(_storage);
					ptr->handle_request(_http_request_parser, response);
					return ptr;
				} else {
					keepalive = false;
					DefaultServerResponses::get_response(404, false, response);
				}
			} else {
				keepalive = false;
				DefaultServerResponses::get_response(400, false, response);
			}

			return nullptr;
		}

		virtual ~WebSessionHandlerFactory() {
		}
	};
}




#endif /* SRC_WEBSESSIONHANDLERFACTORY_H_ */
