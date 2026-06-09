/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponseMock.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:02 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/08 17:18:12 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef HTTP_RESPONSE_MOCK_HPP
#define HTTP_RESPONSE_MOCK_HPP
#include "../../http/IHttpResponse.hpp"

class HttpResponseMock : public IHttpResponse {
	std::string mocked_response;

public:
	HttpResponseMock(std::string mocked_response) : mocked_response(mocked_response) {
	}
	HttpResponseMock() : mocked_response("") {
	}
	~HttpResponseMock() {
	}
	HttpResponseMock& operator=(HttpResponseMock const& other) {
		if (this == &other)
			return *this;
		this->mocked_response = other.mocked_response;
		return *this;
	}
	void setStatus(int code) {
		(void)code;
	}
	void setHeader(const std::string& name, const std::string& value) {
		(void)name;
		(void)value;
	}
	void setBody(const std::string& body) {
		(void)body;
	}
	std::string serialize() const {
		return mocked_response;
	}
};
#endif
