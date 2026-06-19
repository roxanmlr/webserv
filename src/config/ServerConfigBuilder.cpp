/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfigBuilder.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:30 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/18 23:12:20 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfigBuilder.hpp"

ServerConfigBuilder::ServerConfigBuilder()
	: listen_addresses(), server_names(), root_dir(), indexes(), error_pages(), location_configs(), client_max_body_size(), timeOut(),
	  _hasDirectoryList(false) {
}

ServerConfigBuilder::ServerConfigBuilder(ServerConfigBuilder const& other)
	: listen_addresses(), server_names(), root_dir(), indexes(), error_pages(), location_configs(), client_max_body_size(), timeOut(),
	  _hasDirectoryList(false) {
	*this = other;
}

ServerConfigBuilder& ServerConfigBuilder::operator=(ServerConfigBuilder const& other) {
	if (this == &other)
		return *this;
	this->listen_addresses = other.listen_addresses;
	this->server_names	   = other.server_names;
	this->root_dir		   = other.root_dir;
	this->indexes		   = other.indexes;
	this->error_pages	   = other.error_pages;
	this->timeOut		   = other.timeOut;
	for (std::vector<ILocationConfig*>::const_iterator it = location_configs.begin(); it != location_configs.end(); ++it)
		delete *it;
	location_configs.clear();
	for (std::vector<ILocationConfig*>::const_iterator it = other.location_configs.begin(); it != other.location_configs.end(); ++it)
		location_configs.push_back((*it)->clone());
	this->client_max_body_size = other.client_max_body_size;
	return *this;
}

ServerConfigBuilder::~ServerConfigBuilder() {
	for (std::vector<ILocationConfig*>::const_iterator it = location_configs.begin(); it != location_configs.end(); ++it)
		delete *it;
}

ServerConfigBuilder& ServerConfigBuilder::addListenAddress(IServerConfig::ListenAddress address) {
	if (address.host.empty())
		throw ConfigError("listen address host cannot be empty");
	if (address.port == 0)
		throw ConfigError("listen address port cannot be zero");
	listen_addresses.push_back(address);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::addServerName(std::string server_name) {
	for (std::vector<std::string>::const_iterator it = server_names.begin(); it != server_names.end(); ++it) {
		if (*it == server_name)
			throw ConfigError("server name `" + server_name + "` is duplicated in configuration");
	}
	server_names.push_back(server_name);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::setRootDir(std::string root_dir) {
	if (root_dir.empty())
		throw ConfigError("root_dir cannot be empty");
	if (!this->root_dir.empty())
		throw ConfigError("root_dir is already filled");
	this->root_dir.set(root_dir);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::addIndex(std::string index) {
	for (std::vector<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); ++it) {
		if (*it == index)
			throw ConfigError("index `" + index + "` is duplicated in configuration");
	}
	indexes.push_back(index);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::addErrorPage(IServerConfig::ErrorPage error_page) {
	error_pages.push_back(error_page);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::addLocation(ILocationConfig* location) {
	for (std::vector<ILocationConfig*>::const_iterator it = location_configs.begin(); it != location_configs.end(); ++it) {
		if (*location == **it)
			throw ConfigError("location is duplicated in configuration");
	}
	location_configs.push_back(location);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::setClientMaxBodySize(std::size_t maxsize) {
	if (!this->client_max_body_size.empty())
		throw ConfigError("client_max_body_size is already filled");
	this->client_max_body_size.set(maxsize);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::setTimeOut(size_t timeOut) {
	if (!this->timeOut.empty())
		throw ConfigError("timeOut is already filled");
	this->timeOut.set(timeOut);
	return *this;
}

ServerConfigBuilder& ServerConfigBuilder::hasDirectoryList() {
	if (_hasDirectoryList)
		throw ConfigError("Duplicate directory_list directive");
	_hasDirectoryList = true;
	return *this;
}

IServerConfig* ServerConfigBuilder::build() {
	if (root_dir.empty()) {
		throw ConfigError("No root directory provided");
	}
	if (location_configs.empty()) {
		LocationConfigBuilder lcfgBuilder;
		lcfgBuilder.setPath("/");
		lcfgBuilder.addAllowedMethod(ILocationConfig::GET);
		location_configs.push_back(lcfgBuilder.build());
	}
	return new ServerConfig(listen_addresses, server_names, root_dir, indexes, error_pages, location_configs, client_max_body_size, timeOut, _hasDirectoryList);
}
