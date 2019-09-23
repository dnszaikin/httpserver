/*
 * PollingHelper.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_POLLINGHELPER_H_
#define SRC_POLLINGHELPER_H_

#include <vector>
#include <unordered_map>
#include <sys/poll.h>
#include <unistd.h>

#include "CommonUtils.h"
#include "ISocket.h"
#include "IHandlerFactory.h"

namespace network {

	template <class T>
	class PollingHelper {
	private:
		std::vector<pollfd> _poll_fds; //it is a not best idea to use map in poll reactor, in production app better to use array
		std::unordered_map<int, ISocket::ptr> _clients; //track clients by socket

		std::shared_ptr<ISocket> _last_client;

	public:
		PollingHelper() {

		};

		void add_client(std::shared_ptr<ISocket> socket) {
			pollfd client_pollfd;

			client_pollfd.fd = socket->get_socket();
			client_pollfd.events = POLLIN;

			_poll_fds.emplace_back(client_pollfd);

			_clients.emplace(socket->get_socket(), socket);

			_last_client = socket;
		};

		void add_client(int socket, const sockaddr& addr, IHandlerFactory::ptr handler) {
			pollfd client_pollfd;

			client_pollfd.fd = socket;
			client_pollfd.events = POLLIN;

			_poll_fds.emplace_back(client_pollfd);

			auto&& ret = get_name_info(addr);

			auto&& clsckt = std::make_shared<T>();

			clsckt->init(ret.first, ret.second, socket, handler);

			_clients.emplace(socket, clsckt);

			_last_client = clsckt;
		};

		const std::shared_ptr<ISocket>& get_last_client() const {
			return _last_client;
		}

		std::shared_ptr<ISocket> get_client(int socket) const {

			auto&& it = _clients.find(socket);

			if (it != _clients.end()) {
				auto&& sp = it->second.lock();
				return sp;
			}

			throw std::runtime_error("Socket " + std::to_string(socket) + " is not found");
		}

		const std::unordered_map<int, ISocket::ptr>& get_clients() const {
			return _clients;
		}

		std::vector<pollfd>& get_polling() {
			return _poll_fds;
		}

		void delete_client(std::vector<pollfd>::iterator it) {
			LOG_INFO("Removing client... " << it->fd);
			auto&& _it = _clients.find(it->fd);

			if (_it != _clients.end()) {
				LOG_INFO(_it->second.use_count());
				_it->second.reset();
				_clients.erase(_it);
				_poll_fds.erase(it);
			} else {
				LOG_DEBUG("Unexpected: Requesting to delete not existing socket " << socket << ". Possible leak.");
			}
		}

		size_t get_size() {
			return _clients.size();
		}

		virtual ~PollingHelper() {

		};
	};

}


#endif /* SRC_POLLINGHELPER_H_ */
