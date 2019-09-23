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
#include <mutex>

namespace network {

constexpr uint64_t READ_BUF_SIZE = 2048;

class UnixClientSocket: public network::AbstractSocket {
private:

	byte_vector _receive_buffer;
	byte_vector _send_buffer;
	size_t _send_count;
	std::mutex _send_buffer_mutex;
	std::mutex _receive_buffer_mutex;

public:
	UnixClientSocket(): AbstractSocket(), _send_count(0) {
		_receive_buffer.reserve(READ_BUF_SIZE);

	} ;

	/*
	 * append data to outgoing buffer
	 */
	void append_data_to_send(const byte_vector& data) override {
		std::lock_guard<std::mutex> lock(_send_buffer_mutex);

		_send_buffer.insert(_send_buffer.end(), data.begin(), data.end());
	}

	/*
	 * swap received data with user buffer
	 */
	void swap_received(byte_vector& data) override {
		std::lock_guard<std::mutex> lock(_receive_buffer_mutex);

		_receive_buffer.swap(data);
	}

	void init(std::string_view host, std::string_view port, int socket_fd, IHandlerFactory::ptr handler) override {
		AbstractSocket::init(host, port, socket_fd, handler);

		int opt = 1;
		ioctl(get_socket(), FIONBIO, &opt);

		set_connected();
	}

	virtual ~UnixClientSocket() {};

	callback begin_send() override {
		bool close_connection = false;
		bool complete = false;

		ssize_t size = 0;
		if (is_connected() && _send_buffer.size() > 0) {

			byte_vector send_buffer;

			{
				std::lock_guard<std::mutex> lock(_send_buffer_mutex);
				send_buffer.swap(_send_buffer);
			}

			LOG_DEBUG(get_name() << ": sending " << send_buffer.size() << " bytes, raw [" << std::string_view(&send_buffer.data()[0], send_buffer.size()) << "]");

			size = ::send(get_socket(), send_buffer.data(), send_buffer.size(), MSG_NOSIGNAL);

			if (size < 0) {
				if (errno != EWOULDBLOCK || errno != EAGAIN) {
					LOG_ERROR(get_name() << ": send failed. Error: " << strerr());
					close_connection = true;
					complete = true;
				}
//				break;
			} else if (size < send_buffer.size()){

				byte_vector tmp;
				tmp.assign(send_buffer.data()[0]+size, send_buffer.size() - size);
				send_buffer.swap(tmp);

				{
					std::lock_guard<std::mutex> lock(_send_buffer_mutex);
					send_buffer.insert(send_buffer.end(), _send_buffer.begin(), _send_buffer.end());
					_send_buffer.swap(send_buffer);
				}
			} else if (size == send_buffer.size()) {
				complete = true;
//				break;
			} else if (size == 0){
				LOG_INFO(get_name() << ": client disconnected.");
				close_connection = true;
				complete = true;
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
				   complete = true;
			   }
//			   break;
			}
			//client disconnected
			else if (size == 0) {
				close_connection = true;
				complete = true;
				LOG_INFO(get_name() << ": client disconnected.");
//				break;
			} else if (size == READ_BUF_SIZE) {
				byte_vector recv_buff(buffer, buffer + size);

				{
					std::lock_guard<std::mutex> lock(_receive_buffer_mutex);
					_receive_buffer.insert(_receive_buffer.end(), recv_buff.begin(), recv_buff.end());
				}
			} else if (size < READ_BUF_SIZE) {
				byte_vector recv_buff(buffer, buffer + size);
				{
					std::lock_guard<std::mutex> lock(_receive_buffer_mutex);
					_receive_buffer.insert(_receive_buffer.end(), recv_buff.begin(), recv_buff.end());
				}
		    	complete = true;
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
			if (size == 0) {
				return;
			}

			LOG_DEBUG(get_name() << ": received " << _receive_buffer.size() << " bytes, raw ["
					<< std::string_view(&_receive_buffer.data()[0],	_receive_buffer.size()) << "]");
			LOG_DEBUG(get_name() << ": receive complete!");
		}
	}

	void end_send(bool complete, size_t size) override {
		if (size == 0) {
			return;
		}

		if (!complete) {
			_send_count += size;
			LOG_DEBUG(get_name() << ": sent " << size << " from " << _send_buffer.size() << " bytes");
		} else {
	    	_send_count += size;
			LOG_DEBUG(get_name() << ": send complete! Sent: " << _send_count << " bytes");
			_send_buffer.clear();
			_send_count = 0;
		}
	}
};

}

#endif /* SRC_UNIXCLIENTSOCKET_H_ */
#endif
