/*
 * NetworkUtils.h
 *
 *  Created on: Sep 20, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_COMMONUTILS_H_
#define SRC_COMMONUTILS_H_

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <locale>


#ifndef _WIN32
#include <string.h>
#include <errno.h>
#include <netdb.h>
#endif

#include "Logger.h"

namespace utils::network {

#ifndef _WIN32
	inline std::string strerr() {
		return std::string(strerror(errno));
	}

	/*
	 * converts sockaddress to host and port representation. returns pair first is host, last is port
	 */
	inline std::pair<std::string_view, std::string_view> get_name_info(const struct sockaddr& address) {

		char hostname[128];
		char port[128];
		if(::getnameinfo(&address, sizeof(sockaddr), hostname, sizeof(hostname),
				port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV) != 0){
			LOG_ERROR("Failed to translate socket to client address. Error: " << strerr());
		}

		return std::make_pair<std::string_view, std::string_view>(hostname, port);
	}
#endif


	inline std::string get_http_date() {
	    auto now = std::chrono::system_clock::now();
	    auto in_time_t = std::chrono::system_clock::to_time_t(now);

	    std::stringstream ss;
	    ss << std::put_time(std::localtime(&in_time_t), "%a, %d %b %Y %H:%M:%S %Z");

		return ss.str();
	}
}

namespace utils::common {

	constexpr unsigned int str2int(const char* str, int h = 0)
	{
		return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
	}

	inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	inline std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}

	inline std::vector<std::string> split(const std::string &s, char delim) {
	  std::stringstream ss(s);
	  std::string item;
	  std::vector<std::string> elems;

	  while (std::getline(ss, item, delim)) {
		  elems.emplace_back(item);
	  }

	  return elems;
	}
}


#endif /* SRC_COMMONUTILS_H_ */