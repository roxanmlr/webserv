/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:05:39 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/09 16:05:40 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include <ctime>

std::string formatTime(time_t t) {
	char	   buf[20];
	struct tm* tm_info = localtime(&t);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
	return buf;
}

Logger::Logger() {
}

Logger::~Logger() {
}

Logger::Logger(Logger const& other) {
	(void)other;
}

Logger& Logger::operator=(Logger const& other) {
	(void)other;
	return *this;
}

void Logger::log(Level level, const std::string& msg) {
	std::string tag;
	switch (level) {
	case DEBUG:
		tag = "DEBUG";
		break;
	case INFO:
		tag = "INFO";
		break;
	case WARN:
		tag = "WARN";
		break;
	case ERROR:
		tag = "ERROR";
		break;
	}
	std::cerr << "[" << formatTime(time(NULL)) << "] " << tag << " ---- " << msg << std::endl;
}
