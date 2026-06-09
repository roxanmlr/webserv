/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestMock.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:09 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/08 22:16:15 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTP_REQUEST_MOCK_HPP
#define HTTP_REQUEST_MOCK_HPP
#include "../../http/IHttpRequest.hpp"
#include <exception>
#include <map>
// Les données arrivent en morceaux via le poll
class HttpRequestMock : public IHttpRequest {
private:
	std::string						   method;
	std::string						   uri;
	std::map<std::string, std::string> headers;
	std::string						   body;
	int								   error_code;

public:
	HttpRequestMock(std::string method, std::string uri, std::map<std::string, std::string> headers, std::string body, int error_code)
		: method(method), uri(uri), headers(headers), body(body), error_code(error_code) {
	}
	~HttpRequestMock() {
	}
	HttpRequestMock& operator=(HttpRequestMock const& other) {
		if (this == &other)
			return *this;
		this->method  = other.method;
		this->uri	  = other.uri;
		this->headers = other.headers;
		this->body	  = other.body;
		return *this;
	}
	IHttpRequest::ParseState feed(const char* data, std::size_t len) {
		if (len == __SIZE_MAX__)
			return PARSE_ERROR;
		if (data)
			return INCOMPLETE;
		return COMPLETE;
	}
	const std::string& getMethod() const {
		return method;
	}
	const std::string& getUri() const {
		return uri;
	}
	const std::string& getHeader(const std::string& name) const {
		if (headers.count(name) > 0)
			return headers.at(name);
		throw new std::exception();
	}
	const std::string& getBody() const {
		return body;
	}
	int getErrorCode() const {
		return error_code;
	}
};
#endif
