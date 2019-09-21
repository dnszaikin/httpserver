/*
 * UnixClientSocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_UNIXCLIENTSOCKET_H_
#define SRC_UNIXCLIENTSOCKET_H_

#include <vector>
#include "AbstractSocket.h"

namespace network {

constexpr uint64_t READ_BUF_SIZE = 2028;

class UnixClientSocket: public network::AbstractSocket {
private:
	std::string _name;

	std::string_view get_name() {
		return _name;
	}

public:
	UnixClientSocket(): AbstractSocket() {} ;

	UnixClientSocket(const UnixClientSocket&): AbstractSocket() {} ;
	void init(std::string_view host, std::string_view port, int socket_fd) override {
		set_host(host);
		set_port(port);
		set_socket(socket_fd);
		set_connected();
		_name = std::string(host) + ":" + std::string(port);

		int opt = 1;
		ioctl(get_socket(), FIONBIO, &opt);
	}

	virtual ~UnixClientSocket() {};

	void send() override {
		bool close_connection = false;

		while (is_connected()) {
			std::vector<char> buf;

			ssize_t size = ::send(get_socket(), buf.data(), buf.size(), 0);

			if (size < 0) {
				if (errno != EWOULDBLOCK || errno != EAGAIN) {
					LOG_ERROR(get_name() << ": send failed. Error: " << strerr());
					close_connection = true;
				}
				break;
			} else if (size < buf.size()){
				LOG_DEBUG(get_name() << ": sending " << buf.size() << " bytes, raw [" << std::string_view(&buf.data()[0], size) << "]");

				std::vector<char> tmp;
				tmp.assign(buf.data()[0]+size, buf.size() - size);
				buf.swap(tmp);

				LOG_DEBUG(get_name() << ": remaining " << buf.size() << " bytes, raw [" << std::string_view(&buf.data()[0], buf.size()) << "]");
			} else if (size == buf.size()) {

				LOG_DEBUG(get_name() << ": sent " << buf.size() << " bytes, raw [" << std::string_view(&buf.data()[0], buf.size()) << "]");
				break;
			} else if (size == 0){
				LOG_INFO(get_name() << ": client1 disconnected.");
				close_connection = true;
				break;
			}
		}

		if (close_connection) {
		   shutdown();
		   close();
		}
	}

	void recv() override {
		bool close_connection = false;

		char buffer[READ_BUF_SIZE];

		std::vector<char> received_buffer;

		received_buffer.clear();
		received_buffer.reserve(READ_BUF_SIZE);

		while (is_connected()) {

			ssize_t size = ::recv(get_socket(), &buffer, READ_BUF_SIZE, 0);

			//error when read
			if (size < 0) {
			   if (errno != EWOULDBLOCK || errno != EAGAIN) {
				   LOG_ERROR(get_name() << ": recv failed. Error: " << strerr());
				   close_connection = true;
			   }
			   break;
			}
			//client disconnected
			else if (size == 0) {
				close_connection = true;
				LOG_INFO(get_name() << ": client disconnected.");
				break;
			} else if (size == READ_BUF_SIZE) {
				std::vector<char> recv_buff(buffer, buffer + size);
		    	received_buffer.insert(received_buffer.end(), recv_buff.begin(), recv_buff.end());
		    	LOG_DEBUG(get_name() << ": received " << size << " bytes, raw [" << std::string_view(&received_buffer.data()[0], received_buffer.size()) << "]");
			} else if (size < READ_BUF_SIZE) {
				std::vector<char> recv_buff(buffer, buffer + size);
		    	received_buffer.insert(received_buffer.end(), recv_buff.begin(), recv_buff.end());

				LOG_DEBUG(get_name() << ": received " << size << " bytes, raw [" << std::string_view(&received_buffer.data()[0], received_buffer.size()) << "]");
				break;
			}
		}

		if (close_connection) {
		   shutdown();
		   close();
		}
	}
};

}

#endif /* SRC_UNIXCLIENTSOCKET_H_ */
#endif
