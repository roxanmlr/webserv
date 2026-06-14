/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:25 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/11 14:22:18 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IRequestHandler.hpp"

class StaticFileHandler : public IRequestHandler {
private:
public:
	virtual ~StaticFileHandler();
	StaticFileHandler();
	StaticFileHandler(const StaticFileHandler& base);
	StaticFileHandler& operator=(const StaticFileHandler& base);

	virtual bool	   canHandle(const IHttpRequest& req, const ILocationConfig& loc, IServerConfig const * serv);
	virtual bool	   handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
};
