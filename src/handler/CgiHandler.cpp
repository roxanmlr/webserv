/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mzouhir <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:28:23 by mzouhir           #+#    #+#             */
/*   Updated: 2026/06/11 15:30:18 by mzouhir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <fstream>
#include <sstream>

CgiHandler::CgiHandler() {
}

CgiHandler::~CgiHandler() {
}

CgiHandler::CgiHandler(const CgiHandler& base) {
}

CgiHandler& CgiHandler::operator=(const CgiHandler& base) {
	if (this == &base)
		return *this;
	return (*this);
}

bool CgiHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	// TODO stripe server_name first
	if (req.getUri().find_last_of('.') == std::string::npos)
		return false;
	/*return req.getUri().find(cgipass.extension, req.getUri().find_last_of('.')) && req.getUri().find(loc.getPath()) != std::string::npos &&
		   !loc.getCgiPasses().empty();*/
	for (std::vector<ILocationConfig::CgiPass>::const_iterator it = loc.getCgiPasses().begin(); it != loc.getCgiPasses().end(); ++it) {
		if (req.getUri().find((*it).extension, req.getUri().find_last_of('.')) && req.getUri().find(loc.getPath()) != std::string::npos) {
			this->cgipass = (*it);
			return true;
		}
	}
	return false;
}

bool CgiHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	if (cgipass.extension == "" || cgipass.interpreter == "")
		return false;
	int pipefd[2];
	int outfile[2];

	if (pipe(pipefd) == -1)
		throw WebServerError("pipe failure");
	if (pipe(outfile) == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		throw WebServerError("pipe failure");
	}
	if (!fork()) {
		close(pipefd[1]);
		close(outfile[0]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1) {
			close(pipefd[0]);
			close(outfile[1]);
			exit(EXIT_FAILURE);
		}
		if (dup2(outfile[1], STDOUT_FILENO) == -1) {
			close(pipefd[0]);
			close(outfile[1]);
			exit(EXIT_FAILURE);
		}
		const char* path	= cgipass.interpreter.c_str();
		char*		args[2] = {const_cast<char*>(path), NULL};
		char**		envp	= (char**)calloc(req.getAllHeaders().size() + 1, sizeof(char*));
		if (!envp) {
			close(pipefd[0]);
			close(outfile[1]);
			exit(EXIT_FAILURE);
		}
		int i = 0;
		for (std::map<std::string, std::string>::const_iterator it = req.getAllHeaders().begin(); it != req.getAllHeaders().end();++it){
			std::stringstream env_string;
			env_string << (*it).first << "=" << (*it).second;
			envp[i] = const_cast<char *>(env_string.str().c_str());
			i++;
		}
		envp[i] = NULL;
		execve(path, args, envp);
		free(envp);
		exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	close(outfile[1]);
	/*
	TODO write body on pipefd[1]
	TODO read output on outfile[0];
	*/
	int wstatus;
	waitpid(-1, &wstatus, NULL); // TODO check return value
	(void)wstatus;
	return (true);
}
