/*
 * Logger.h
 *
 *  Created on: Sep 19, 2019
 *      Author: dnszaikin
 */

#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <iostream>

namespace Logger {
static bool DEBUG = true;
}

#define LOG_INFO(msg) std::cout << msg << std::endl
#define LOG_ERROR(msg) std::cout << msg << std::endl
#define LOG_DEBUG(msg) if (Logger::DEBUG) {std::cout << "[DBG] " << __FILE__ << ":" << __LINE__ << "\t" << msg << std::endl;}

#endif /* SRC_LOGGER_H_ */
