/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IServerConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:51 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:47:52 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ISERVER_CONFIG_HPP
#define ISERVER_CONFIG_HPP
#include "../utils/Optional.tpp"
#include "ConfigError.hpp"
#include <ostream>
#include <string>
#include <vector>
class ILocationConfig;

class IServerConfig {
public:
	struct ListenAddress {
		std::string	 host;
		unsigned int port;
	};
	struct ErrorPage {
		std::vector<unsigned int> codes;
		std::string				  path;
	};
	virtual ~IServerConfig() {
	}
	virtual IServerConfig*						 clone() const									  = 0;
	virtual std::vector<ListenAddress> const&	 getListenAddresses() const						  = 0;
	virtual std::vector<std::string> const&		 getServerNames() const							  = 0;
	virtual Optional<std::string> const&		 getRootDir() const								  = 0;
	virtual std::vector<std::string> const&		 getIndexes() const								  = 0;
	virtual std::vector<ErrorPage> const&		 getErrorPages() const							  = 0;
	virtual std::vector<ILocationConfig*> const& getLocations() const							  = 0;
	virtual Optional<std::size_t> const&		 getClientMaxBodySize() const					  = 0;
	virtual bool								 matchesServerName(const std::string& name) const = 0;
	virtual Optional<ILocationConfig const*>	 matchLocation(const std::string& uri) const	  = 0;
};

std::ostream& operator<<(std::ostream& out, IServerConfig const& srv);
#endif
