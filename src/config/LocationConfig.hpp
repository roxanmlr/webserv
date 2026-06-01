/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:34 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:35 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP
#include "ILocationConfig.hpp"
#include <map>

class LocationConfig : public ILocationConfig {
private:
	std::string								path;
	ILocationConfig::MatchType				match_type;
	Optional<std::string>					root;
	std::vector<std::string>				index_files;
	std::vector<IServerConfig::ErrorPage>	error_pages;
	std::vector<ILocationConfig::Method>	allowed_methods;
	bool									auto_index;
	Optional<std::size_t>					client_max_body_size;
	Optional<ILocationConfig::ReturnConfig> return_config;
	Optional<std::string>					upload_store;
	Optional<std::string>					fastcgi_pass;
	std::map<std::string, std::string>		fastcgi_params;
	std::vector<ILocationConfig::CgiPass>	cgi_passes;

public:
	LocationConfig();
	LocationConfig(std::string path, ILocationConfig::MatchType match_type, Optional<std::string> root, std::vector<std::string> index_files,
				   std::vector<IServerConfig::ErrorPage> error_pages, std::vector<ILocationConfig::Method> allowed_methods, bool auto_index,
				   Optional<std::size_t> client_max_body_size, Optional<ILocationConfig::ReturnConfig> return_config, Optional<std::string> upload_store,
				   Optional<std::string> fastcgi_pass, std::map<std::string, std::string> fastcgi_params, std::vector<ILocationConfig::CgiPass> cgi_passes);
	~LocationConfig();
	LocationConfig(LocationConfig const& other);
	LocationConfig&								   operator=(LocationConfig const& other);

	ILocationConfig*							   clone() const;
	const std::string&							   getPath() const;
	ILocationConfig::MatchType					   getMatchType() const;
	bool										   matches(const std::string& uri) const;
	Optional<std::string> const&				   getRoot() const;
	Optional<std::size_t> const&				   getClientMaxBodySize() const;
	std::vector<std::string> const&				   getIndexFiles() const;
	std::vector<IServerConfig::ErrorPage> const&   getErrorPages() const;
	std::vector<ILocationConfig::Method> const&	   getAllowedMethods() const;
	bool										   isAutoIndex() const;
	Optional<ILocationConfig::ReturnConfig> const& getReturnConfig() const;
	Optional<std::string> const&				   getUploadStore() const;
	Optional<std::string> const&				   getFastCgiPass() const;
	std::map<std::string, std::string> const&	   getFastCgiParams() const;
	std::vector<ILocationConfig::CgiPass> const&   getCgiPasses() const;
	bool										   isMethodAllowed(const std::string& method) const;
	bool										   operator==(ILocationConfig const& other) const;
};
#endif
