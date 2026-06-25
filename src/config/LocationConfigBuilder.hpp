/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigBuilder.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:38 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/25 20:33:51 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef LOCATION_CONFIG_BUILDER_HPP
#define LOCATION_CONFIG_BUILDER_HPP
#include "IServerConfig.hpp"
#include "LocationConfig.hpp"
#include <map>

class LocationConfigBuilder {
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
	Optional<std::string>					auth_filename;
	Optional<bool>							auth_active;

public:
	LocationConfigBuilder();
	~LocationConfigBuilder();
	LocationConfigBuilder(LocationConfigBuilder const& other);
	LocationConfigBuilder& operator=(LocationConfigBuilder const& other);

	LocationConfigBuilder& setPath(std::string path);
	LocationConfigBuilder& setMatchType(ILocationConfig::MatchType match_type);
	LocationConfigBuilder& setRoot(std::string root);
	LocationConfigBuilder& addIndexFile(std::string index_file);
	LocationConfigBuilder& addErrorPage(IServerConfig::ErrorPage error_page);
	LocationConfigBuilder& addAllowedMethod(ILocationConfig::Method method);
	LocationConfigBuilder& setAutoIndex(bool auto_index);
	LocationConfigBuilder& setClientMaxBodySize(std::size_t client_max_body_size);
	LocationConfigBuilder& setReturnConfig(ILocationConfig::ReturnConfig return_config);
	LocationConfigBuilder& setUploadStore(std::string upload_store);
	LocationConfigBuilder& setTimeOut(size_t seconds);
	LocationConfigBuilder& setFastCgiPass(std::string fastcgi_pass);
	LocationConfigBuilder& addFastCgiParam(std::string key, std::string value);
	LocationConfigBuilder& addCgiPass(ILocationConfig::CgiPass cgi_pass);
	LocationConfigBuilder& setAuthBasicUserFile(std::string filename);
	LocationConfigBuilder& setAuthSwitch(bool activate);
	ILocationConfig*	   build();
};
#endif
