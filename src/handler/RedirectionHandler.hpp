/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectionHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:03:56 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/17 14:21:03 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../http/HttpResponse.hpp"
#include "IRequestHandler.hpp"

class RedirectionHandler : public IRequestHandler {
private:
public:
	~RedirectionHandler();
	RedirectionHandler();
	RedirectionHandler(const RedirectionHandler& base);
	RedirectionHandler& operator=(const RedirectionHandler& base);

	bool				canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	bool				handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, const IServerConfig* serv);
};
