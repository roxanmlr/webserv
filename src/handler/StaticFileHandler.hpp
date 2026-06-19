/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:25 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/19 16:38:48 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IRequestHandler.hpp"

class StaticFileHandler : public IRequestHandler {
private:
	void generateDirectoryListing(const std::string & path, const std::string & uri, IHttpResponse & res);
public:
	virtual ~StaticFileHandler();
	StaticFileHandler();
	StaticFileHandler(const StaticFileHandler& base);
	StaticFileHandler& operator=(const StaticFileHandler& base);

	virtual bool	   canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	virtual bool	   handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
};
