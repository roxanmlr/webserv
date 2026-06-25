/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:06 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/25 22:23:14 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <sstream>

LocationConfig::LocationConfig()
	: path(""), match_type(ILocationConfig::MATCH_PREFIX), root(), index_files(), error_pages(), allowed_methods(), auto_index(false), client_max_body_size(),
	  return_config(), upload_store(), fastcgi_pass(), fastcgi_params(), cgi_passes(), auth_filename(), auth_activate(false) {
}

LocationConfig::LocationConfig(std::string path, ILocationConfig::MatchType match_type, Optional<std::string> root, std::vector<std::string> index_files,
							   std::vector<IServerConfig::ErrorPage> error_pages, std::vector<ILocationConfig::Method> allowed_methods, bool auto_index,
							   Optional<std::size_t> client_max_body_size, Optional<ILocationConfig::ReturnConfig> return_config,
							   Optional<std::string> upload_store, Optional<std::string> fastcgi_pass, std::map<std::string, std::string> fastcgi_params,
							   std::vector<ILocationConfig::CgiPass> cgi_passes, Optional<std::string> auth_filename, bool auth_activate)
	: path(path), match_type(match_type), root(root), index_files(index_files), error_pages(error_pages), allowed_methods(allowed_methods),
	  auto_index(auto_index), client_max_body_size(client_max_body_size), return_config(return_config), upload_store(upload_store), fastcgi_pass(fastcgi_pass),
	  fastcgi_params(fastcgi_params), cgi_passes(cgi_passes), auth_filename(auth_filename), auth_activate(auth_activate) {
}

LocationConfig::~LocationConfig() {
}

LocationConfig::LocationConfig(LocationConfig const& other)
	: path(other.path), match_type(other.match_type), root(other.root), index_files(other.index_files), error_pages(other.error_pages),
	  allowed_methods(other.allowed_methods), auto_index(other.auto_index), client_max_body_size(other.client_max_body_size),
	  return_config(other.return_config), upload_store(other.upload_store), fastcgi_pass(other.fastcgi_pass), fastcgi_params(other.fastcgi_params),
	  cgi_passes(other.cgi_passes) {
}

LocationConfig& LocationConfig::operator=(LocationConfig const& other) {
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
	this->auth_activate		   = other.auth_activate;
	this->auth_filename		   = other.auth_filename;
	return *this;
}

ILocationConfig* LocationConfig::clone() const {
	return new LocationConfig(*this);
}

const std::string& LocationConfig::getPath() const {
	return path;
}

ILocationConfig::MatchType LocationConfig::getMatchType() const {
	return match_type;
}

bool LocationConfig::matches(const std::string& uri) const {
	if (match_type == ILocationConfig::MATCH_EXACT)
		return uri == path;
	if (uri.size() < path.size())
		return false;
	if (uri.compare(0, path.size(), path) != 0)
		return false;
	if (!path.empty() && path[path.size() - 1] == '/')
		return true;
	return uri.size() == path.size() || uri[path.size()] == '/';
}

Optional<std::string> const& LocationConfig::getRoot() const {
	return root;
}

Optional<std::size_t> const& LocationConfig::getClientMaxBodySize() const {
	return client_max_body_size;
}

std::vector<std::string> const& LocationConfig::getIndexFiles() const {
	return index_files;
}

std::vector<IServerConfig::ErrorPage> const& LocationConfig::getErrorPages() const {
	return error_pages;
}

std::vector<ILocationConfig::Method> const& LocationConfig::getAllowedMethods() const {
	return allowed_methods;
}

bool LocationConfig::isAutoIndex() const {
	return auto_index;
}

Optional<ILocationConfig::ReturnConfig> const& LocationConfig::getReturnConfig() const {
	return return_config;
}

Optional<std::string> const& LocationConfig::getUploadStore() const {
	return upload_store;
}

Optional<std::string> const& LocationConfig::getFastCgiPass() const {
	return fastcgi_pass;
}

std::map<std::string, std::string> const& LocationConfig::getFastCgiParams() const {
	return fastcgi_params;
}

std::vector<ILocationConfig::CgiPass> const& LocationConfig::getCgiPasses() const {
	return cgi_passes;
}

bool LocationConfig::needsAuthentication() const {
	return auth_activate;
}

Optional<std::string> LocationConfig::getAuthFilename() const {
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

std::ostream& operator<<(std::ostream& out, ILocationConfig const& loc) {
	out << "    location ";
	if (loc.getMatchType() == ILocationConfig::MATCH_EXACT)
		out << "= ";
	else if (loc.getMatchType() == ILocationConfig::MATCH_PREFIX_PRIORITY)
		out << "^~ ";
	out << loc.getPath() << " {\n";
	if (!loc.getRoot().empty())
		out << "        root " << loc.getRoot().get() << ";\n";
	if (!loc.getClientMaxBodySize().empty())
		out << "        client_max_body_size " << sizeToStr(loc.getClientMaxBodySize().get()) << ";\n";
	
	if (!loc.getAuthFilename().empty())
		out << "auth_basic_user_file " << loc.getAuthFilename().get() << ";\n";
	if (!loc.needsAuthentication())
		out << "        auth_basic no\n";
	else 
		out << "        auth_basic yes\n";
	const std::vector<std::string>& idx = loc.getIndexFiles();
	if (!idx.empty()) {
		out << "        index";
		for (std::vector<std::string>::const_iterator it = idx.begin(); it != idx.end(); ++it)
			out << " " << *it;
		out << ";\n";
	}
	const std::vector<IServerConfig::ErrorPage>& pages = loc.getErrorPages();
	for (std::vector<IServerConfig::ErrorPage>::const_iterator it = pages.begin(); it != pages.end(); ++it) {
		out << "        error_page";
		for (std::vector<unsigned int>::const_iterator c = it->codes.begin(); c != it->codes.end(); ++c)
			out << " " << *c;
		out << " " << it->path << ";\n";
	}
	const std::vector<ILocationConfig::Method>& methods = loc.getAllowedMethods();
	if (!methods.empty()) {
		out << "        allow_methods";
		for (std::vector<ILocationConfig::Method>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
			switch (*it) {
			case ILocationConfig::GET:
				out << " GET";
				break;
			case ILocationConfig::POST:
				out << " POST";
				break;
			case ILocationConfig::DELETE:
				out << " DELETE";
				break;
			case ILocationConfig::PUT:
				out << " PUT";
				break;
			}
		}
		out << ";\n";
	}
	if (loc.isAutoIndex())
		out << "        autoindex on;\n";
	if (!loc.getReturnConfig().empty()) {
		out << "        return " << loc.getReturnConfig().get().code;
		out << " " << loc.getReturnConfig().get().url << ";\n";
	}
	if (!loc.getUploadStore().empty())
		out << "        upload_store " << loc.getUploadStore().get() << ";\n";
	if (!loc.getFastCgiPass().empty())
		out << "        fastcgi_pass " << loc.getFastCgiPass().get() << ";\n";
	const std::map<std::string, std::string>& params = loc.getFastCgiParams();
	for (std::map<std::string, std::string>::const_iterator it = params.begin(); it != params.end(); ++it)
		out << "        fastcgi_param " << it->first << " " << it->second << ";\n";
	const std::vector<ILocationConfig::CgiPass>& cgis = loc.getCgiPasses();
	for (std::vector<ILocationConfig::CgiPass>::const_iterator it = cgis.begin(); it != cgis.end(); ++it)
		out << "        cgi_pass " << it->extension << " " << it->interpreter << ";\n";
	out << "    }\n";
	return out;
}

bool LocationConfig::isMethodAllowed(const std::string& method) const {
	if (allowed_methods.empty())
		return method == "GET";
	ILocationConfig::Method m;
	if (method == "GET")
		m = ILocationConfig::GET;
	else if (method == "POST")
		m = ILocationConfig::POST;
	else if (method == "DELETE")
		m = ILocationConfig::DELETE;
	else if (method == "PUT")
		m = ILocationConfig::PUT;
	else
		return false;
	for (std::vector<ILocationConfig::Method>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) {
		if (*it == m)
			return true;
	}
	return false;
}

bool LocationConfig::operator==(ILocationConfig const& other) const {
	return path == other.getPath() && match_type == other.getMatchType();
}
