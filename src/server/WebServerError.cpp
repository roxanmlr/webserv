/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerError.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:24:43 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/08 22:28:33 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServerError.hpp"

WebServerError::WebServerError() : message("WebServer Error : ") {
}

WebServerError::WebServerError(const std::string& message) : message("WebServer Error : " + message) {
}

WebServerError::WebServerError(WebServerError const& other) : message(other.message) {
}

WebServerError& WebServerError::operator=(WebServerError const& other) {
	if (this == &other)
		return *this;
	this->message = other.message;
	return *this;
}

WebServerError::~WebServerError() throw() {
}

const char* WebServerError::what() const throw() {
	return this->message.c_str();
}
