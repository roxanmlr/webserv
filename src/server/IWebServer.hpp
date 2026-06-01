/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IWebServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:32 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:33 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IWEB_SERVER_HPP
#define IWEB_SERVER_HPP
#include "../config/IConfig.hpp"

class IWebServer {
public:
	virtual ~IWebServer() {
	}
	virtual void init(const IConfig& config) = 0;
	virtual void run()						 = 0;
	virtual void stop()						 = 0;
};
#endif
