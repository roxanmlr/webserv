/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 18:05:03 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 17:17:55 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../config/IServerConfig.hpp"
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

	static void			applyErrorPage(IHttpResponse& res, int statusCode, const IServerConfig* serv);
};
