/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IFastCgiClient.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:47 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:48 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IFAST_CGI_CLIENT_HPP
#define IFAST_CGI_CLIENT_HPP
#include "../config/ILocationConfig.hpp"
#include "../http/IHttpRequest.hpp"
#include <string>

class IFastCgiClient {
public:
	enum State { CONNECTING, SENDING, RECEIVING, FINISHED, ERROR };
	virtual ~IFastCgiClient() {
	}
	virtual int				   getFd() const											  = 0;
	virtual State			   getState() const											  = 0;
	virtual void			   onReadable()												  = 0;
	virtual void			   onWritable()												  = 0;
	virtual bool			   wantsRead() const										  = 0;
	virtual bool			   wantsWrite() const										  = 0;
	virtual void			   start(const IHttpRequest& req, const ILocationConfig& loc) = 0;
	virtual const std::string& getResponseHeaders() const								  = 0;
	virtual const std::string& getResponseBody() const									  = 0;
};
#endif
