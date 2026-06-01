/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerError.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:24:43 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/01 20:24:44 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServerError.hpp"

WebServerError::WebServerError() : message("") {
}

WebServerError::WebServerError(const std::string& message) : message(message) {
}

WebServerError::WebServerError(WebServerError const& other) : message(other.message) {
}

WebServerError& WebServerError::operator=(WebServerError const& other) {
	if (this == &other)
		return *this;
	*this = other;
	return *this;
}

WebServerError::~WebServerError() throw() {
}

const char* WebServerError::what() const throw() {
	return this->message.c_str();
}
