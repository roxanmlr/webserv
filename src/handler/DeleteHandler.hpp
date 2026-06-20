/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 14:28:37 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 14:41:01 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IRequestHandler.hpp"

class DeleteHandler : public IRequestHandler {
private:
public:
	DeleteHandler();
	virtual ~DeleteHandler();

	DeleteHandler(const DeleteHandler& base);
	DeleteHandler& operator=(const DeleteHandler& base);

	virtual bool   canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	virtual bool   handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, const IServerConfig* serv);
};
