/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RedirectionHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 11:03:59 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/20 16:24:41 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RedirectionHandler.hpp"

RedirectionHandler::~RedirectionHandler() {
}

RedirectionHandler::RedirectionHandler() {
}

RedirectionHandler::RedirectionHandler(const RedirectionHandler& base) {
	(void)base;
}

RedirectionHandler& RedirectionHandler::operator=(const RedirectionHandler& base) {
	(void)base;
	return (*this);
}

bool RedirectionHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	if (loc.getReturnConfig().empty())
		return false;
	std::string loc_root(loc.getPath());
	if (!loc.getRoot().empty())
		loc_root = loc.getRoot().get();
	std::cerr << "Redirect : " << req.getUri() << "\n";
	return (req.getUri().find(loc_root, 0) == 0);
}

bool RedirectionHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	(void)serv;
	std::cerr << "Redirection handler\n";
	int code = loc.getReturnConfig().get().code;
	res.setStatus(code);
	std::string location = req.getUri();
	std::string loc_root(loc.getPath());
	if (!loc.getRoot().empty())
		loc_root = loc.getRoot().get();
	size_t pos = location.find(loc_root);
	if (pos == 0) {
		location.replace(pos, loc_root.length(), loc.getReturnConfig().get().url);
	}
	res.setHeader("Location", location);
	//	HttpResponse::applyErrorPage(res, code, serv);
	return (true);
}
