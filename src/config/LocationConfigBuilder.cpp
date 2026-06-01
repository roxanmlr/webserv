/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfigBuilder.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:07 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:47:09 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfigBuilder.hpp"

LocationConfigBuilder::LocationConfigBuilder()
	: path(), match_type(ILocationConfig::MATCH_PREFIX), root(), index_files(), error_pages(), allowed_methods(), auto_index(false), client_max_body_size(),
	  return_config(), upload_store(), fastcgi_pass(), fastcgi_params(), cgi_passes() {
}

LocationConfigBuilder::~LocationConfigBuilder() {
}

LocationConfigBuilder::LocationConfigBuilder(LocationConfigBuilder const& other)
	: path(other.path), match_type(other.match_type), root(other.root), index_files(other.index_files), error_pages(other.error_pages),
	  allowed_methods(other.allowed_methods), auto_index(other.auto_index), client_max_body_size(other.client_max_body_size),
	  return_config(other.return_config), upload_store(other.upload_store), fastcgi_pass(other.fastcgi_pass), fastcgi_params(other.fastcgi_params),
	  cgi_passes(other.cgi_passes) {
}

LocationConfigBuilder& LocationConfigBuilder::operator=(LocationConfigBuilder const& other) {
	if (this == &other)
		return *this;
	this->path				   = other.path;
	this->match_type		   = other.match_type;
	this->root				   = other.root;
	this->index_files		   = other.index_files;
	this->error_pages		   = other.error_pages;
	this->allowed_methods	   = other.allowed_methods;
	this->auto_index		   = other.auto_index;
	this->client_max_body_size = other.client_max_body_size;
	this->return_config		   = other.return_config;
	this->upload_store		   = other.upload_store;
	this->fastcgi_pass		   = other.fastcgi_pass;
	this->fastcgi_params	   = other.fastcgi_params;
	this->cgi_passes		   = other.cgi_passes;
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setPath(std::string path) {
	if (path.empty())
		throw ConfigError("Can't set a empty path");
	if (!this->path.empty())
		throw ConfigError("Path is already filled");
	this->path = path;
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setMatchType(ILocationConfig::MatchType match_type) {
	this->match_type = match_type;
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setRoot(std::string root) {
	if (root.empty())
		throw ConfigError("Can't set a empty root");
	if (!this->root.empty())
		throw ConfigError("root is already filled");
	this->root.set(root);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::addIndexFile(std::string index_file) {
	for (std::vector<std::string>::const_iterator it = index_files.begin(); it != index_files.end(); ++it) {
		if (*it == index_file)
			throw ConfigError("Index file `" + index_file + "` is duplicated in configuration");
	}
	index_files.push_back(index_file);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::addErrorPage(IServerConfig::ErrorPage error_page) {
	error_pages.push_back(error_page);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::addAllowedMethod(ILocationConfig::Method method) {
	for (std::vector<ILocationConfig::Method>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
		if (*it == method)
			throw ConfigError("Duplicate allowed method in configuration");
	}
	allowed_methods.push_back(method);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setAutoIndex(bool auto_index) {
	this->auto_index = auto_index;
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setClientMaxBodySize(std::size_t client_max_body_size) {
	if (!this->client_max_body_size.empty())
		throw ConfigError("client_max_body_size is already filled");
	this->client_max_body_size.set(client_max_body_size);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setReturnConfig(ILocationConfig::ReturnConfig return_config) {
	if (!this->return_config.empty())
		throw ConfigError("return_config is already filled");
	this->return_config.set(return_config);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setUploadStore(std::string upload_store) {
	if (upload_store.empty())
		throw ConfigError("Can't set a empty upload_store");
	if (!this->upload_store.empty())
		throw ConfigError("upload_store is already filled");
	this->upload_store.set(upload_store);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::setFastCgiPass(std::string fastcgi_pass) {
	if (fastcgi_pass.empty())
		throw ConfigError("Can't set a empty fastcgi_pass");
	if (!this->fastcgi_pass.empty())
		throw ConfigError("fastcgi_pass is already filled");
	this->fastcgi_pass.set(fastcgi_pass);
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::addFastCgiParam(std::string key, std::string value) {
	if (fastcgi_params.find(key) != fastcgi_params.end())
		throw ConfigError("FastCGI param `" + key + "` is duplicated in configuration");
	fastcgi_params[key] = value;
	return *this;
}

LocationConfigBuilder& LocationConfigBuilder::addCgiPass(ILocationConfig::CgiPass cgi_pass) {
	for (std::vector<ILocationConfig::CgiPass>::const_iterator it = cgi_passes.begin(); it != cgi_passes.end(); ++it) {
		if (it->extension == cgi_pass.extension)
			throw ConfigError("CGI pass for extension `" + cgi_pass.extension + "` is duplicated in configuration");
	}
	cgi_passes.push_back(cgi_pass);
	return *this;
}

ILocationConfig* LocationConfigBuilder::build() {
	return new LocationConfig(path, match_type, root, index_files, error_pages, allowed_methods, auto_index, client_max_body_size, return_config, upload_store,
							  fastcgi_pass, fastcgi_params, cgi_passes);
}
