/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:04:20 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/09 16:01:00 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IHttpRequest.hpp"
#include <map>

class HttpRequest : public IHttpRequest {
private:
	enum StateMachine { REQUEST_LINE, HEADER, BODY, COMPLETE, ERROR };
	std::string						   _buffer;
	StateMachine					   _state;
	std::string						   _method;
	std::string						   _uri;
	std::map<std::string, std::string> _header;
	std::string						   _body;
	int								   _errorCode;
	std::size_t						   _contentLength;

	bool							   parseRequestLine(const std::string& line);
	bool							   parseHeaderLine(const std::string& line);

public:
	HttpRequest(void);
	virtual ~HttpRequest(void);
	HttpRequest(const HttpRequest& base);
	HttpRequest&									  operator=(const HttpRequest& base);

	virtual ParseState								  feed(const char* data, std::size_t len);
	virtual const std::string&						  getMethod(void) const;
	virtual const std::string&						  getUri(void) const;
	virtual const std::string&						  getHeader(const std::string& name) const;
	virtual const std::map<std::string, std::string>& getAllHeaders() const;
	virtual const std::string&						  getBody(void) const;
	virtual int										  getErrorCode(void) const;
	const std::string&								  getBuffer(void) const;
};
