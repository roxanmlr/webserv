/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ICgiHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:39 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:39 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ICGI_HANDLER_HPP
#define ICGI_HANDLER_HPP
#include "../handler/IRequestHandler.hpp"

class ICgiHandler : public IRequestHandler {
public:
	virtual ~ICgiHandler() {
	}
	virtual int	 getInputFd() const												= 0; // pipe vers STDIN du CGI
	virtual int	 getOutputFd() const											= 0; // pipe vers STDOUT du CGI
	virtual bool isFinished()													= 0;
	virtual void onInput()														= 0; // appelé sur POLLOUT du pipe stdin
	virtual void onOutput()														= 0; // appelé sur POLLOUT du pipe stdout
	virtual bool canHandle(const IHttpRequest& req, const ILocationConfig& loc) = 0;
	virtual bool handle(const IHttpRequest&, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) = 0;
	virtual void fillResponse(IHttpResponse& res)																		= 0;
};
#endif
