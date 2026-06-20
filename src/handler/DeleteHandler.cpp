/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DeleteHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 14:28:38 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 14:57:39 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "DeleteHandler.hpp"

#include <sys/stat.h>
#include <unistd.h>

DeleteHandler::DeleteHandler() {
}

DeleteHandler::~DeleteHandler() {
}

DeleteHandler::DeleteHandler(const DeleteHandler& base) {
	(void)base;
}

DeleteHandler& DeleteHandler::operator=(const DeleteHandler& base) {
	(void)base;
	return (*this);
}

bool DeleteHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	if (req.getMethod() != "DELETE")
		return (false);
	return (loc.isMethodAllowed("DELETE"));
}

bool DeleteHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, const IServerConfig* serv) {
	std::string rootPath = "";
	if (!serv->getRootDir().empty())
		rootPath = serv->getRootDir().get();
	if (!loc.getRoot().empty())
		rootPath = loc.getRoot().get();

	std::string path = rootPath + req.getUri();

	struct stat file;
	if (stat(path.c_str(), &file) == -1) {
		res.setStatus(404);
		res.setBody("<h1>404 Not Found</h1>");
		return (true);
	}

	if (S_ISDIR(file.st_mode)) {
		res.setStatus(403);
		res.setBody("<h1>403 Forbidden: Cannot delete directory</h1>");
		return (true);
	}

	if (unlink(path.c_str()) == 0)
		res.setStatus(204);
	else {
		res.setStatus(403);
		res.setBody("<h1>403 Forbidden: Cannot delete file</h1>");
	}
	return (true);
}
