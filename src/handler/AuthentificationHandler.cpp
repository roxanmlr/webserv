/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AuthentificationHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:03:59 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 16:24:41 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AuthentificationHandler.hpp"

AuthentificationHandler::~AuthentificationHandler() {
}

AuthentificationHandler::AuthentificationHandler() {
}

AuthentificationHandler::AuthentificationHandler(const AuthentificationHandler& base) {
	(void)base;
}

AuthentificationHandler& AuthentificationHandler::operator=(const AuthentificationHandler& base) {
	(void)base;
	return (*this);
}

bool AuthentificationHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	return loc.needsAuthentication();
}

bool AuthentificationHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	std::string filename("");
	if (serv->getAuthFilename().empty() && loc.getAuthFilename().empty())
		return false;
	if (req.getAllHeaders().count("Authorization") == 0) {
		res.setStatus(401);
		res.setHeader("WWW-Authenticate", "Basic realm=\"private\"");
		return true;
	}
	std::string auth_header = req.getHeader("Authorization");
	if (!serv->getAuthFilename().empty())
		filename = serv->getAuthFilename().get();
	if (!loc.getAuthFilename().empty())
		filename = loc.getAuthFilename().get();
	std::string rootPath("");
	if (!serv->getRootDir().empty())
		rootPath = serv->getRootDir().get();
	if (!loc.getRoot().empty())
		rootPath = loc.getRoot().get();
	filename = rootPath + filename;
	std::ifstream file(filename.c_str());
	if (!file.good()) {
		HttpResponse::applyErrorPage(res, 500, serv);
		return true;
	}
	std::string line;
	while (std::getline(file, line)) {
		size_t pos = line.find(':');
		if (pos != std::string::npos) {
		}
	}
	return (true);
}
