/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServerSocket.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:27 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:28 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ISERVER_SOCKET_HPP
#define ISERVER_SOCKET_HPP
#include "../config/IServerConfig.hpp"
class IServerSocket {
public:
	virtual ~IServerSocket() {
	}
	virtual int								   getFd() const	  = 0;
	virtual int								   acceptClient()	  = 0;
	virtual const std::vector<IServerConfig*>& getConfigs() const = 0;
};
#endif
