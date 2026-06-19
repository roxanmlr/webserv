/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticFileHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:23 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/19 17:00:39 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticFileHandler.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

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

bool StaticFileHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	(void)loc;
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
		if (!indexFound)
		{
			if (serv->hasDirectoryList())
			{
				generateDirectoryListing(path, req.getUri(), res);
				return (true);
			}
			else
			{
				res.setStatus(403);
				res.setBody("<h1>403 Forbidden</h1>");
				return (true);
			}
		}

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

void StaticFileHandler::generateDirectoryListing(const std::string & path, const std::string & uri, IHttpResponse & res)
{
	DIR* dir = opendir(path.c_str());
	if (!dir)
	{
		res.setStatus(403);
		res.setBody("<h1>403 Forbidden</h1>");
		return ;
	}

	std::stringstream ss;
	ss << "<html><head><title>Index of " << uri << "</title></head><body>";
	ss << "<h1>Index of " << uri << "</h1><hr><pre>";
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == ".")
			continue;
		std::string link = name;

		if (entry->d_type == DT_DIR)
			link+= "/";
		ss << "<a href=\"" << link << "\">" << link << "</a><br>";
	}
	ss << "</pre><hr></body></html>";
	closedir(dir);
	res.setStatus(200);
	res.setBody(ss.str());
}
