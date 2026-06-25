/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:13 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/25 22:23:36 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"
#include "ILocationConfig.hpp"
#include <sstream>

ServerConfig::ServerConfig()
	: listen_addresses(), server_names(), root_dir(), indexes(), error_pages(), location_configs(), client_max_body_size(), timeOut(),
	  _hasDirectoryList(false) {
}

ServerConfig::ServerConfig(std::vector<IServerConfig::ListenAddress> listen_addresses, std::vector<std::string> server_names, Optional<std::string> root_dir,
						   std::vector<std::string> indexes, std::vector<IServerConfig::ErrorPage> error_pages, std::vector<ILocationConfig*> location_configs,
						   Optional<std::size_t> client_max_body_size, Optional<std::size_t> timeOut, bool _hasDirectoryList,
						   Optional<std::string> auth_filename)
	: listen_addresses(listen_addresses), server_names(server_names), root_dir(root_dir), indexes(indexes), error_pages(error_pages), location_configs(),
	  client_max_body_size(client_max_body_size), timeOut(timeOut), _hasDirectoryList(_hasDirectoryList), auth_filename(auth_filename) {
	for (std::vector<ILocationConfig*>::iterator it = location_configs.begin(); it != location_configs.end(); ++it)
		this->location_configs.push_back((*it)->clone());
}

ServerConfig::~ServerConfig() {
	for (std::vector<ILocationConfig*>::iterator it = location_configs.begin(); it != location_configs.end(); ++it)
		delete *it;
}

ServerConfig::ServerConfig(ServerConfig const& other)
	: listen_addresses(), server_names(), root_dir(), indexes(), error_pages(), location_configs(), client_max_body_size(), auth_filename() {
	*this = other;
}

ServerConfig& ServerConfig::operator=(ServerConfig const& other) {
	if (this == &other)
		return *this;
	this->listen_addresses	= other.listen_addresses;
	this->server_names		= other.server_names;
	this->root_dir			= other.root_dir;
	this->indexes			= other.indexes;
	this->error_pages		= other.error_pages;
	this->timeOut			= other.timeOut;
	this->_hasDirectoryList = other._hasDirectoryList;
	for (std::vector<ILocationConfig*>::iterator it = location_configs.begin(); it != location_configs.end(); ++it)
		delete *it;
	location_configs.clear();
	for (std::vector<ILocationConfig*>::const_iterator it = other.location_configs.begin(); it != other.location_configs.end(); ++it)
		this->location_configs.push_back((*it)->clone());
	this->client_max_body_size = other.client_max_body_size;
	this->auth_filename		   = other.auth_filename;
	return *this;
}

IServerConfig* ServerConfig::clone() const {
	return new ServerConfig(*this);
}

std::vector<IServerConfig::ListenAddress> const& ServerConfig::getListenAddresses() const {
	return listen_addresses;
}

std::vector<std::string> const& ServerConfig::getServerNames() const {
	return server_names;
}

Optional<std::string> const& ServerConfig::getRootDir() const {
	return root_dir;
}

std::vector<std::string> const& ServerConfig::getIndexes() const {
	return indexes;
}

std::vector<IServerConfig::ErrorPage> const& ServerConfig::getErrorPages() const {
	return error_pages;
}

std::vector<ILocationConfig*> const& ServerConfig::getLocations() const {
	return location_configs;
}

Optional<size_t> ServerConfig::getTimeOut() const {
	return timeOut;
}

Optional<std::size_t> const& ServerConfig::getClientMaxBodySize() const {
	return client_max_body_size;
}

bool ServerConfig::matchesServerName(const std::string& name) const {
	for (std::vector<std::string>::const_iterator it = server_names.begin(); it != server_names.end(); ++it) {
		if (*it == name)
			return true;
	}
	return false;
}

bool ServerConfig::hasDirectoryList() const {
	return _hasDirectoryList;
}

Optional<std::string> ServerConfig::getAuthFilename() const {
	return auth_filename;
}

static std::string sizeToStr(std::size_t sz) {
	std::ostringstream oss;
	if (sz != 0 && sz % (1024UL * 1024 * 1024) == 0)
		oss << sz / (1024UL * 1024 * 1024) << "G";
	else if (sz != 0 && sz % (1024UL * 1024) == 0)
		oss << sz / (1024UL * 1024) << "M";
	else if (sz != 0 && sz % 1024 == 0)
		oss << sz / 1024 << "K";
	else
		oss << sz;
	return oss.str();
}

std::ostream& operator<<(std::ostream& out, IServerConfig const& srv) {
	out << "server {\n";
	const std::vector<IServerConfig::ListenAddress>& addrs = srv.getListenAddresses();
	for (std::vector<IServerConfig::ListenAddress>::const_iterator it = addrs.begin(); it != addrs.end(); ++it)
		out << "    listen " << it->host << ":" << it->port << ";\n";
	const std::vector<std::string>& names = srv.getServerNames();
	if (!names.empty()) {
		out << "    server_name";
		for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
			out << " " << *it;
		out << ";\n";
	}
	if (!srv.getRootDir().empty())
		out << "    root " << srv.getRootDir().get() << ";\n";
	if (!srv.getClientMaxBodySize().empty())
		out << "    client_max_body_size " << sizeToStr(srv.getClientMaxBodySize().get()) << ";\n";
	const std::vector<std::string>& indexes = srv.getIndexes();
	if (!indexes.empty()) {
		out << "    index";
		for (std::vector<std::string>::const_iterator it = indexes.begin(); it != indexes.end(); ++it)
			out << " " << *it;
		out << ";\n";
	}
	const std::vector<IServerConfig::ErrorPage>& pages = srv.getErrorPages();
	for (std::vector<IServerConfig::ErrorPage>::const_iterator it = pages.begin(); it != pages.end(); ++it) {
		out << "    error_page";
		for (std::vector<unsigned int>::const_iterator c = it->codes.begin(); c != it->codes.end(); ++c)
			out << " " << *c;
		out << " " << it->path << ";\n";
	}
	if (!srv.getAuthFilename().empty())
		out << "    auth_basic_user_file " << srv.getAuthFilename().get() << ";\n";
	const std::vector<ILocationConfig*>& locs = srv.getLocations();
	for (std::vector<ILocationConfig*>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
		out << "\n";
		out << **it;
	}
	out << "}\n";
	return out;
}

Optional<ILocationConfig const*> ServerConfig::matchLocation(const std::string& uri) const {
	Optional<ILocationConfig const*> ret;
	std::size_t						 longestMatch = 0;
	for (std::vector<ILocationConfig*>::const_iterator it = location_configs.begin(); it != location_configs.end(); ++it) {
		if ((*it)->matches(uri)) {
			if ((*it)->getMatchType() == ILocationConfig::MATCH_EXACT) {
				ret.set(*it);
				return ret;
			}

			std::size_t currentLen = (*it)->getPath().length();
			if (currentLen > longestMatch) {
				longestMatch = currentLen;
				ret.set(*it);
			}
		}
	}
	return ret;
}
