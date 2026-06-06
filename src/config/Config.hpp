/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:22 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/05 22:57:10 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "IConfig.hpp"
#include <istream>
#include <ostream>

class Config : public IConfig {
private:
	std::vector<IServerConfig*> server_configs;
	bool						is_correct;

public:
	Config();
	~Config();
	Config(Config const& other);
	Config&							   operator=(Config const& other);
	const std::vector<IServerConfig*>& getServers() const;
	const IServerConfig*			   findServer(const std::string& host, unsigned int port) const;
	void							   addServer(IServerConfig*);
	bool							   good() const;
	std::istream&					   read(std::istream& in);
	std::ostream&					   write(std::ostream& out) const;
};

#endif
