/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRequestHandler.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:58 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/18 11:34:25 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IREQUEST_HANDLER_HPP
#define IREQUEST_HANDLER_HPP
#include "../config/ILocationConfig.hpp"
#include "../http/IHttpRequest.hpp"
#include "../http/IHttpResponse.hpp"

class IRequestHandler {
public:
	virtual ~IRequestHandler() {
	}
	virtual bool canHandle(const IHttpRequest& req, const ILocationConfig& loc)											= 0;
	virtual bool handle(const IHttpRequest&, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) = 0;
};
#endif
