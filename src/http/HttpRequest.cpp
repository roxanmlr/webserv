/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:04:17 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/09 16:01:36 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <sstream>

HttpRequest::HttpRequest(void) : _state(REQUEST_LINE), _errorCode(0), _contentLength(0) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest& HttpRequest::operator=(const HttpRequest& base) {
	if (this != &base) {
		this->_buffer = base._buffer;
		this->_state  = base._state;
		this->_method = base._method;
		this->_uri	  = base._uri;
		this->_header.clear();
		this->_header		 = base._header;
		this->_body			 = base._body;
		this->_errorCode	 = base._errorCode;
		this->_contentLength = base._contentLength;
	}
	return (*this);
}

HttpRequest::HttpRequest(const HttpRequest& base) {
	*this = base;
}

const std::string& HttpRequest::getMethod(void) const {
	return (this->_method);
}

const std::string& HttpRequest::getUri(void) const {
	return (this->_uri);
}

const std::string& HttpRequest::getHeader(const std::string& name) const {
	std::map<std::string, std::string>::const_iterator it;
	it = this->_header.find(name);

	if (it == this->_header.end()) {
		static const std::string empty;
		return (empty);
	}
	return (it->second);
}

const std::map<std::string, std::string>& HttpRequest::getAllHeaders() const {
	return _header;
}
const std::string& HttpRequest::getBody(void) const {
	return (this->_body);
}

int HttpRequest::getErrorCode(void) const {
	return (this->_errorCode);
}

const std::string& HttpRequest::getBuffer(void) const {
	return (this->_buffer);
}

bool HttpRequest::parseRequestLine(const std::string& line) {
	std::stringstream str(line);
	std::string		  method;
	std::string		  uri;
	std::string		  version;
	std::string		  test;

	if (!(str >> method) || !(str >> uri) || !(str >> version))
		return (false);

	if (str >> test)
		return (false);

	if (version != "HTTP/1.1")
		return (false);

	if (uri.empty() || uri[0] != '/')
		return (false);

	this->_method = method;
	this->_uri	  = uri;
	return (true);
}

bool HttpRequest::parseHeaderLine(const std::string& line) {
	std::string key;
	std::string value;
	size_t		pos;

	pos = line.find(":");
	if (pos == std::string::npos)
		return (false);
	key			 = line.substr(0, pos);
	value		 = line.substr(pos + 1);
	size_t first = value.find_first_not_of(" ");
	size_t last	 = value.find_last_not_of(" ");
	if (first != std::string::npos && last != std::string::npos)
		value = value.substr(first, last - first + 1);
	else
		value = "";
	if (key == "Content-Length") {
		if (!(value.empty()) && value[0] == '-')
			return (false);
		std::stringstream ss(value);
		if (!(ss >> this->_contentLength))
			return (false);
		if (!(ss.eof()))
			return (false);
	}
	this->_header[key] = value;
	return (true);
}

IHttpRequest::ParseState HttpRequest::feed(const char* data, std::size_t len) {
	if (this->_state == COMPLETE)
		return (IHttpRequest::COMPLETE);
	if (this->_state == ERROR)
		return (IHttpRequest::PARSE_ERROR);
	this->_buffer.append(data, len);
	while (this->_state == REQUEST_LINE || this->_state == HEADER) {
		size_t pos = 0;
		pos		   = this->_buffer.find("\r\n");
		if (pos == std::string::npos)
			return (IHttpRequest::INCOMPLETE);
		std::string line;
		line = this->_buffer.substr(0, pos);
		this->_buffer.erase(0, pos + 2);
		if (this->_state == REQUEST_LINE) {
			if (this->parseRequestLine(line) == true)
				this->_state = HEADER;
			else {
				this->_state	 = ERROR;
				this->_errorCode = 400;
				return (IHttpRequest::PARSE_ERROR);
			}
		} else if (this->_state == HEADER) {
			if (line.empty()) {
				if (this->_contentLength > 0)
					this->_state = BODY;
				else {
					this->_state = COMPLETE;
					return (IHttpRequest::COMPLETE);
				}
			} else {
				if (this->parseHeaderLine(line) == false) {
					this->_state	 = ERROR;
					this->_errorCode = 400;
					return (IHttpRequest::PARSE_ERROR);
				}
			}
		}
	}
	if (this->_state == BODY) {
		if (this->_buffer.size() >= this->_contentLength) {
			this->_body = this->_buffer.substr(0, this->_contentLength);
			this->_buffer.erase(0, this->_contentLength);
			this->_state = COMPLETE;
			return (IHttpRequest::COMPLETE);
		}
	}
	return (IHttpRequest::INCOMPLETE);
}
