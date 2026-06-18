/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:03:59 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/17 14:56:44 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UploadHandler.hpp"
#include <fstream>

UploadHandler::~UploadHandler() {
}

UploadHandler::UploadHandler() {
}

UploadHandler::UploadHandler(const UploadHandler& base) {
	(void)base;
}

UploadHandler& UploadHandler::operator=(const UploadHandler& base) {
	(void)base;
	return (*this);
}

bool UploadHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	if (req.getMethod() != "POST")
		return (false);
	if (loc.getUploadStore().empty())
		return (false);
	return (true);
}

bool UploadHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	(void)serv;
	std::string uri(req.getUri());
	std::size_t pos = uri.rfind('/');
	std::string fileName;
	if (pos == std::string::npos)
		fileName = uri;
	else
		fileName = uri.substr(pos + 1);

	if (fileName.empty()) {
		res.setStatus(400);
		res.setBody("<h1>400 Bad Request: Missing Filename</h1>");
		return (true);
	}

	std::string path = loc.getUploadStore().get();
	if (!path.empty() && path[path.length() - 1] != '/')
		path += '/';
	path += fileName;
	std::ofstream outfile(path.c_str(), std::ios::out | std::ios::binary);
	if (!outfile.is_open()) {
		res.setStatus(500);
		res.setBody("<h1>500 Internal Server Error</h1>");
		return (true);
	}
	outfile.write(req.getBody().c_str(), req.getBody().size());
	outfile.close();
	res.setStatus(201);
	res.setBody("<h1>201 Created</h1>");
	return (true);
}
