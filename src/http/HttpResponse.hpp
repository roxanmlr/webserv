/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:05:03 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/10 18:34:25 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IHttpResponse.hpp"
#include <map>

class HttpResponse : public IHttpResponse {
private:
	int								   _status;
	std::map<std::string, std::string> _header;
	std::string						   _body;
	std::string						   _getCodeError(int code) const;

public:
	virtual ~HttpResponse(void);
	HttpResponse(void);
	HttpResponse(const HttpResponse& base);
	HttpResponse&		operator=(const HttpResponse& base);

	virtual void		setStatus(int code);
	virtual void		setHeader(const std::string& name, const std::string& value);
	virtual void		setBody(const std::string& body);
	virtual std::string serialize() const;
};
