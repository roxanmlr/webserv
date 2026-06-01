/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:18 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/28 00:00:33 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigError.hpp"

ConfigError::ConfigError() : message("") {
}

ConfigError::ConfigError(const std::string& message) : message(message) {
}

ConfigError::ConfigError(ConfigError const& other) {
	if (this == &other)
		return;
	*this = other;
}

ConfigError& ConfigError::operator=(ConfigError const& other) {
	if (this == &other)
		return *this;
	this->message = other.message;
	return *this;
}

ConfigError::~ConfigError() throw() {
}

const char* ConfigError::what() const throw() {
	return this->message.c_str();
}
