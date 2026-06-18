/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:03:56 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/17 14:21:03 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "IRequestHandler.hpp"

class UploadHandler : public IRequestHandler {
private:
public:
	virtual ~UploadHandler();
	UploadHandler();
	UploadHandler(const UploadHandler& base);
	UploadHandler& operator=(const UploadHandler& base);

	virtual bool   canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	virtual bool   handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, const IServerConfig* serv);
};
