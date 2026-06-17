/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigBuilder.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:57 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:47:58 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ISERVER_CONFIG_BUILDER_HPP
#define ISERVER_CONFIG_BUILDER_HPP
#include "LocationConfigBuilder.hpp"
#include "ServerConfig.hpp"
#include <string>
#include <vector>

class ServerConfigBuilder {
private:
	std::vector<IServerConfig::ListenAddress> listen_addresses;
	std::vector<std::string>				  server_names;
	Optional<std::string>					  root_dir;
	std::vector<std::string>				  indexes;
	std::vector<IServerConfig::ErrorPage>	  error_pages;
	std::vector<ILocationConfig*>			  location_configs;
	Optional<std::size_t>					  client_max_body_size;

public:
	ServerConfigBuilder();
	ServerConfigBuilder(ServerConfigBuilder const& other);
	ServerConfigBuilder& operator=(ServerConfigBuilder const& other);
	~ServerConfigBuilder();
	ServerConfigBuilder& addListenAddress(IServerConfig::ListenAddress address);
	ServerConfigBuilder& addServerName(std::string server_name);
	ServerConfigBuilder& setRootDir(std::string root_dir);
	ServerConfigBuilder& addIndex(std::string index);
	ServerConfigBuilder& addErrorPage(IServerConfig::ErrorPage error_page);
	ServerConfigBuilder& addLocation(ILocationConfig* location);
	ServerConfigBuilder& setClientMaxBodySize(std::size_t maxsize);
	IServerConfig*		 build();
};
#endif
