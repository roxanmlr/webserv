/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:05:05 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 17:18:06 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include <fstream>
#include <sstream>

HttpResponse::~HttpResponse() {
}

HttpResponse::HttpResponse(void) : _status(0) {
	this->setHeader("Content-Length", "0");
}

HttpResponse& HttpResponse::operator=(const HttpResponse& base) {
	if (this != &base) {
		this->_status = base._status;
		this->_header.clear();
		this->_header = base._header;
		this->_body	  = base._body;
	}
	return (*this);
}

HttpResponse::HttpResponse(const HttpResponse& base) : _status(base._status), _header(base._header), _body(base._body) {
}

void HttpResponse::setStatus(int code) {
	this->_status = code;
}

void HttpResponse::setHeader(const std::string& name, const std::string& value) {
	this->_header[name] = value;
}

void HttpResponse::setBody(const std::string& body) {
	this->_body			   = body;
	size_t			  size = body.size();
	std::stringstream ss;
	ss << size;
	std::string res = ss.str();
	this->setHeader("Content-Length", res);
}

std::string HttpResponse::_getCodeError(int code) const {
	switch (code) {
	case 200:
		return ("HTTP/1.1 200 OK\r\n");
		break;
	case 201:
		return ("HTTP/1.1 201 Created\r\n");
		break;
	case 202:
		return ("HTTP/1.1 202 Accepted\r\n");
		break;
	case 204:
		return ("HTTP/1.1 204 No Content\r\n");
		break;
	case 301:
		return ("HTTP/1.1 301 Moved Permanently\r\n");
		break;
	case 400:
		return ("HTTP/1.1 400 Bad Request\r\n");
		break;
	case 403:
		return ("HTTP/1.1 403 Forbidden\r\n");
		break;
	case 404:
		return ("HTTP/1.1 404 Not Found\r\n");
		break;
	case 405:
		return ("HTTP/1.1 405 Method Not Allowed\r\n");
		break;
	case 413:
		return ("HTTP/1.1 413 Payload Too Large\r\n");
		break;
	case 500:
		return ("HTTP/1.1 500 Internal Server Error\r\n");
		break;
	case 502:
		return ("HTTP/1.1 502 Bad Gateway\r\n");
		break;
	case 504:
		return ("HTTP/1.1 504 Gateway Timeout\r\n");
		break;
	default:
		return ("HTTP/1.1 Unknown Error Code\r\n");
		break;
	}
}

std::string HttpResponse::serialize() const {
	std::string res;

	res = this->_getCodeError(this->_status);
	for (std::map<std::string, std::string>::const_iterator it = this->_header.begin(); it != this->_header.end(); ++it) {
		res += it->first;
		res += ": ";
		res += it->second;
		res += "\r\n";
	}
	res += "\r\n";
	res += this->_body;
	return (res);
}

std::string intToString(int n) {
	std::stringstream ss;
	ss << n;
	return (ss.str());
}

void HttpResponse::applyErrorPage(IHttpResponse& res, int statusCode, const IServerConfig* serv) {
	if (!serv) {
		res.setBody("<h1>" + intToString(statusCode) + " Error</h1>");
	}

	const std::vector<IServerConfig::ErrorPage>& pages	 = serv->getErrorPages();
	std::string									 errPath = "";

	for (std::vector<IServerConfig::ErrorPage>::const_iterator it = pages.begin(); it != pages.end(); ++it) {
		for (std::vector<unsigned int>::const_iterator codeIt = it->codes.begin(); codeIt != it->codes.end(); ++codeIt) {
			if (static_cast<int>(*codeIt) == statusCode) {
				errPath = it->path;
				break;
			}
		}
	}

	if (!errPath.empty() && !serv->getRootDir().empty()) {
		std::string	  fullPath = serv->getRootDir().get() + errPath;
		std::ifstream file(fullPath.c_str());
		if (file.is_open()) {
			std::stringstream ss;
			ss << file.rdbuf();
			res.setBody(ss.str());
			return;
		}
	}
	res.setBody("<h1>" + intToString(statusCode) + " Error</h1>");
}
