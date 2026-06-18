/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:25 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/18 11:34:18 by lmilando         ###   ########.fr       */
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

	virtual bool	   canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	virtual bool	   handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
};
