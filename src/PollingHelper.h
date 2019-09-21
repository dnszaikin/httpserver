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

#include "ISocket.h"
#include "NetworkUtils.h"

namespace network {

	template <class T>
	class PollingHelper {
	private:
		std::vector<pollfd> _poll_fds; //it is a not best idea to use map in poll reactor, in production app better to use array
		std::unordered_map<int, ISocket::ptr> _clients; //track clients by socket

		ISocket::ptr _last_client;

	public:
		PollingHelper() {

		};

		void add_client(ISocket::ptr socket) {
			pollfd client_pollfd;

			client_pollfd.fd = socket->get_socket();
			client_pollfd.events = POLLIN;

			_poll_fds.emplace_back(client_pollfd);

			_clients.emplace(socket->get_socket(), socket);

			_last_client = socket;
		};

		void add_client(int socket, const sockaddr& addr) {
			pollfd client_pollfd;

			client_pollfd.fd = socket;
			client_pollfd.events = POLLIN;

			_poll_fds.emplace_back(client_pollfd);

			auto&& ret = get_name_info(addr);

			ISocket::ptr clsckt = std::make_shared<T>();

			clsckt->init(ret.first, ret.second, socket);

			_clients.emplace(socket, clsckt);

			_last_client = clsckt;
		};

		const ISocket::ptr get_last_client() const {
			return _last_client;
		}

		ISocket::ptr get_client(int socket) const {

			auto&& it = _clients.find(socket);

			if (it != _clients.end()) {
				return it->second;
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
			auto&& _it = _clients.find(it->fd);

			if (_it != _clients.end()) {
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
