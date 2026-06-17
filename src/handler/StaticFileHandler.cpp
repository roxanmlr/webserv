/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:23 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/11 15:30:18 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"
#include <fstream>
#include <sstream>

StaticFileHandler::StaticFileHandler() {
}

StaticFileHandler::~StaticFileHandler() {
}

StaticFileHandler::StaticFileHandler(const StaticFileHandler& base) {
	(void)base;
}

StaticFileHandler& StaticFileHandler::operator=(const StaticFileHandler& base) {
	(void)base;
	return (*this);
}

bool StaticFileHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc, IServerConfig const* serv) {
	(void)loc;
	(void)serv;
	if (req.getMethod() != "GET")
		return (false);
	std::string uri = req.getUri();
	if (uri.length() >= 4 && uri.substr(uri.length() - 4) == ".php")
		return (false);
	if (uri.length() >= 3 && uri.substr(uri.length() - 3) == ".py")
		return (false);
	return (true);
}

bool StaticFileHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	std::string rootPath("");
	if (!serv->getRootDir().empty())
		rootPath = serv->getRootDir().get();
	if (!loc.getRoot().empty())
		rootPath = loc.getRoot().get();
	std::string path = rootPath + req.getUri();
	// check for the index path if its a directory from the vector in the config file
	if (!path.empty() && path[path.length() - 1] == '/') {
		const std::vector<std::string>& index	   = loc.getIndexFiles();
		bool							indexFound = false;
		for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it) {
			std::string	  testPath = path + *it;
			std::ifstream testOpen(testPath.c_str());
			if (testOpen.good()) {
				path	   = testPath;
				indexFound = true;
				break;
			}
		}
		if (!indexFound) {
			const std::vector<std::string>& index = serv->getIndexes();
			for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it) {
				std::string	  testPath = path + *it;
				std::ifstream testOpen(testPath.c_str());
				if (testOpen.good()) {
					path	   = testPath;
					indexFound = true;
					break;
				}
			}
		}
		// TODO
		if (indexFound == false && !index.empty())
			path = path + index[0];
	}
	// now we have to construct de response by opening the file found
	std::ifstream file(path.c_str());
	if (!(file.is_open())) {
		res.setStatus(404);
		res.setBody("<h1>404 Not Found</h1>");
		return (true);
	}
	std::stringstream ss;
	ss << file.rdbuf();
	res.setStatus(200);
	res.setBody(ss.str());
	return (true);
}
