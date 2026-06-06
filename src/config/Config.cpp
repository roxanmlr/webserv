/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:26 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/06 12:18:07 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ConfigParser.hpp"
#include <sstream>
Config::Config() : server_configs(), is_correct(false) {
}

Config::~Config() {
	for (std::vector<IServerConfig*>::iterator it = server_configs.begin(); it != server_configs.end(); ++it) {
		IServerConfig* s = *it;
		delete s;
	}
}

Config::Config(Config const& other) {
	if (this == &other)
		return;
	*this = other;
}

Config& Config::operator=(Config const& other) {
	if (this == &other)
		return *this;
	for (std::vector<IServerConfig*>::iterator it = server_configs.begin(); it != server_configs.end(); ++it) {
		delete *it;
	}
	server_configs.clear();
	for (std::vector<IServerConfig*>::const_iterator it = other.server_configs.begin(); it != other.server_configs.end(); ++it) {
		server_configs.push_back((*it)->clone());
	}
	this->is_correct = other.is_correct;
	return *this;
}

const std::vector<IServerConfig*>& Config::getServers() const {
	return server_configs;
}

const IServerConfig* Config::findServer(const std::string& host, unsigned int port) const {
	for (std::vector<IServerConfig*>::const_iterator it = server_configs.begin(); it != server_configs.end(); ++it) {
		std::vector<IServerConfig::ListenAddress> const& addrs = (*it)->getListenAddresses();
		for (std::vector<IServerConfig::ListenAddress>::const_iterator a = addrs.begin(); a != addrs.end(); ++a) {
			if (a->host == host && a->port == port)
				return *it;
		}
	}
	return NULL;
}

std::istream& Config::read(std::istream& in) {
	ConfigParser parser;
	IConfig*	 result = parser.parse(in);
	if (!result) {
		is_correct = false;
		return in;
	}
	is_correct								   = true;
	const std::vector<IServerConfig*>& servers = result->getServers();
	for (std::vector<IServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); ++it){
		IServerConfig* clone = (*it)->clone();	
		try{
		this->addServer(clone);
		}catch(...){
			is_correct = false;
			delete clone;
		}
	}
	delete result;
	return in;
}

std::ostream& Config::write(std::ostream& out) const {
	const std::vector<IServerConfig*>& servers = this->getServers();
	for (std::vector<IServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); ++it) {
		if (it != servers.begin())
			out << "\n";
		out << **it;
	}
	return out;
}

void Config::addServer(IServerConfig* server_config) {
	if (!server_config)
		return;
	std::vector<IServerConfig::ListenAddress> const& new_addrs = server_config->getListenAddresses();
	for (std::vector<IServerConfig*>::iterator it = server_configs.begin(); it != server_configs.end(); ++it) {
		std::vector<IServerConfig::ListenAddress> const& addrs = (*it)->getListenAddresses();
		for (std::vector<IServerConfig::ListenAddress>::const_iterator a = addrs.begin(); a != addrs.end(); ++a) {
			for (std::vector<IServerConfig::ListenAddress>::const_iterator b = new_addrs.begin(); b != new_addrs.end(); ++b) {
				if (a->host == b->host && a->port == b->port) {
					std::stringstream ss;
					ss << "[CONFLICT] Listen address " << a->host << ":" << a->port << " already registered";
					std::string msg = ss.str();
					throw ConfigError(msg);
				}
			}
		}
	}
	server_configs.push_back(server_config);
}

bool Config::good() const {
	return is_correct;
}