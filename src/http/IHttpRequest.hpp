/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpRequest.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:09 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:09 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IHTTP_REQUEST_HPP
#define IHTTP_REQUEST_HPP
#include <string>
// Les données arrivent en morceaux via le poll
class IHttpRequest {
public:
	enum ParseState { INCOMPLETE, COMPLETE, PARSE_ERROR };
	virtual ~IHttpRequest() {
	}
	virtual ParseState		   feed(const char* data, std::size_t len)	= 0;
	virtual const std::string& getMethod() const						= 0;
	virtual const std::string& getUri() const							= 0;
	virtual const std::string& getHeader(const std::string& name) const = 0;
	virtual const std::string& getBody() const							= 0;
	virtual int				   getErrorCode() const						= 0; // si PARSE_ERROR
};
#endif
