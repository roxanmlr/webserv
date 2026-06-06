/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IWebServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:32 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/05 23:15:54 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IWEB_SERVER_HPP
#define IWEB_SERVER_HPP
#include "../config/IConfig.hpp"
#include "../utils/Logger.hpp"

class IWebServer {
public:
	virtual ~IWebServer() {
	}
	virtual void init(const IConfig& config) = 0;
	virtual void run()						 = 0;
	virtual void stop()						 = 0;
};
#endif
