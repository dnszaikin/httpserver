/*
 * UnixClientSocket.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef _WIN32
#ifndef SRC_UNIXCLIENTSOCKET_H_
#define SRC_UNIXCLIENTSOCKET_H_

#include "AbstractSocket.h"
#include "Types.h"

#include <vector>

namespace network {

constexpr uint64_t READ_BUF_SIZE = 2028;

class UnixClientSocket: public network::AbstractSocket {
private:
	std::string _name;

	std::string_view get_name() {
		return _name;
	}

	byte_vector _receive_buffer;
	byte_vector _send_buffer;
	size_t _send_count;

public:
	UnixClientSocket(): AbstractSocket(), _send_count(0) {
		_receive_buffer.reserve(READ_BUF_SIZE);

	} ;

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

	callback begin_send() override {
		bool close_connection = false;
		bool complete = false;

		ssize_t size = 0;
		if (is_connected()) {

			LOG_DEBUG(get_name() << ": sending " << _send_buffer.size() << " bytes, raw [" << std::string_view(&_send_buffer.data()[0], size) << "]");

			size = ::send(get_socket(), _send_buffer.data(), _send_buffer.size(), 0);

			if (size < 0) {
				if (errno != EWOULDBLOCK || errno != EAGAIN) {
					LOG_ERROR(get_name() << ": send failed. Error: " << strerr());
					close_connection = true;
				}
//				break;
			} else if (size < _send_buffer.size()){

				byte_vector tmp;
				tmp.assign(_send_buffer.data()[0]+size, _send_buffer.size() - size);
				_send_buffer.swap(tmp);
			} else if (size == _send_buffer.size()) {
				complete = true;
//				break;
			} else if (size == 0){
				LOG_INFO(get_name() << ": client disconnected.");
				close_connection = true;
//				break;
			}
		}

		if (close_connection) {
		   shutdown();
		   close();
		}

		return std::bind(&UnixClientSocket::end_send, this, complete, size);
	}

	callback begin_recv() override {
		bool close_connection = false;
		bool complete = false;

		char buffer[READ_BUF_SIZE];

		ssize_t size = 0;
		if (is_connected()) {

			 size = ::recv(get_socket(), &buffer, READ_BUF_SIZE, 0);
			//error when read
			if (size < 0) {
			   if (errno != EWOULDBLOCK || errno != EAGAIN) {
				   LOG_ERROR(get_name() << ": recv failed. Error: " << strerr());
				   close_connection = true;
			   }
//			   break;
			}
			//client disconnected
			else if (size == 0) {
				close_connection = true;
				LOG_INFO(get_name() << ": client disconnected.");
//				break;
			} else if (size == READ_BUF_SIZE) {
				byte_vector recv_buff(buffer, buffer + size);
		    	_receive_buffer.insert(_receive_buffer.end(), recv_buff.begin(), recv_buff.end());
		    	_send_count += size;
			} else if (size < READ_BUF_SIZE) {
				byte_vector recv_buff(buffer, buffer + size);
		    	_receive_buffer.insert(_receive_buffer.end(), recv_buff.begin(), recv_buff.end());
		    	complete = true;
		    	_send_count += size;
//				break;
			}
		}

		if (close_connection) {
		   shutdown();
		   close();
		}

		return std::bind(&UnixClientSocket::end_recv, this, complete, size);
	}

	void end_recv(bool complete, size_t size) override {
		if (!complete) {
			LOG_DEBUG(get_name() << ": received " << size << " bytes");
		} else {
			LOG_DEBUG(get_name() << ": received " << _receive_buffer.size() << " bytes, raw ["
					<< std::string_view(&_receive_buffer.data()[0],	_receive_buffer.size()) << "]");
			LOG_DEBUG(get_name() << ": receive complete!");
			_receive_buffer.clear();
		}
	}

	void end_send(bool complete, size_t size) override {
		if (!complete) {
			LOG_DEBUG(get_name() << ": sent " << size << " from " << _send_buffer.size() << " bytes");
		} else {
			LOG_DEBUG(get_name() << ": send complete! Sent: " << _send_count << " bytes");
			_send_buffer.clear();
		}
	}
};

}

#endif /* SRC_UNIXCLIENTSOCKET_H_ */
#endif
