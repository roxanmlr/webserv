/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfig.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:46:52 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/05 22:57:00 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ICONFIG_HPP
#define ICONFIG_HPP
#include "ConfigError.hpp"
#include "IServerConfig.hpp"
#include <ostream>
#include <string>
#include <vector>

class IConfig {
public:
	virtual ~IConfig() {
	}
	virtual const std::vector<IServerConfig*>& getServers() const											= 0;
	virtual const IServerConfig*			   findServer(const std::string& host, unsigned int port) const = 0;
	virtual void							   addServer(IServerConfig*)									= 0;
	virtual bool							   good() const													= 0;
	virtual std::istream&					   read(std::istream& in)										= 0;
	virtual std::ostream&					   write(std::ostream& out) const								= 0;
};

std::ostream& operator<<(std::ostream& out, IConfig const& config);
std::istream& operator>>(std::istream& in, IConfig& config);
#endif
