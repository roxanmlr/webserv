/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:46:58 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:46:59 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP
#include "ILocationConfig.hpp"
#include "IServerConfig.hpp"

class ServerConfig : public IServerConfig {
private:
	std::vector<IServerConfig::ListenAddress> listen_addresses;
	std::vector<std::string>				  server_names;
	Optional<std::string>					  root_dir;
	std::vector<std::string>				  indexes;
	std::vector<IServerConfig::ErrorPage>	  error_pages;
	std::vector<ILocationConfig*>			  location_configs;
	Optional<std::size_t>					  client_max_body_size;

public:
	ServerConfig();
	ServerConfig(std::vector<IServerConfig::ListenAddress> listen_addresses, std::vector<std::string> server_names, Optional<std::string> root_dir,
				 std::vector<std::string> indexes, std::vector<IServerConfig::ErrorPage> error_pages, std::vector<ILocationConfig*> location_configs,
				 Optional<std::size_t> client_max_body_size);
	~ServerConfig();
	ServerConfig(ServerConfig const& other);
	ServerConfig&									 operator=(ServerConfig const& other);
	IServerConfig*									 clone() const;
	std::vector<IServerConfig::ListenAddress> const& getListenAddresses() const;
	std::vector<std::string> const&					 getServerNames() const;
	Optional<std::string> const&					 getRootDir() const;
	std::vector<std::string> const&					 getIndexes() const;
	std::vector<IServerConfig::ErrorPage> const&	 getErrorPages() const;
	std::vector<ILocationConfig*> const&			 getLocations() const;
	Optional<std::size_t> const&					 getClientMaxBodySize() const;
	bool											 matchesServerName(const std::string& name) const;
	Optional<ILocationConfig const*>				 matchLocation(const std::string& uri) const;
};
#endif
