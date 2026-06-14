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
#include <cstring>
#include <fstream>
#include <sstream>

CgiHandler::CgiHandler() {
}

CgiHandler::~CgiHandler() {
}

CgiHandler::CgiHandler(const CgiHandler& base) {
	this->cgipass = base.cgipass;
}

CgiHandler& CgiHandler::operator=(const CgiHandler& base) {
	if (this == &base)
		return *this;
	return (*this);
}

bool CgiHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
	// TODO stripe server_name first
	std::size_t ext_pos = req.getUri().find_last_of('.');
	if (ext_pos == std::string::npos)
		return false;
	for (std::vector<ILocationConfig::CgiPass>::const_iterator it = loc.getCgiPasses().begin(); it != loc.getCgiPasses().end(); ++it) {
		if (req.getUri().find((*it).extension, ext_pos) == ext_pos && req.getUri().find(loc.getPath()) != std::string::npos) {
			this->cgipass = (*it);
			return true;
		}
	}
	return false;
}

bool CgiHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	(void)serv;
	if (!canHandle(req, loc))
		return false;
	int pipefd[2];
	int outfile[2];

	/*TODO add the script path
	the script path come from the request
	No traversal ..*/
	if (pipe(pipefd) == -1)
		throw WebServerError("pipe failure");
	if (pipe(outfile) == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		throw WebServerError("pipe failure");
	}
	int pid = fork();
	if (pid == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		throw WebServerError("fork failure");
	}
	if (!pid) {
		close(pipefd[1]);
		close(outfile[0]);
		if (dup2(pipefd[0], STDIN_FILENO) == -1) {
			close(pipefd[0]);
			close(outfile[1]);
			std::exit(EXIT_FAILURE);
		}
		if (dup2(outfile[1], STDOUT_FILENO) == -1) {
			close(pipefd[0]);
			close(outfile[1]);
			std::exit(EXIT_FAILURE);
		}
		const char* path = cgipass.interpreter.c_str();

		/*TODO	 char* args[3] = {const_cast<char*>(path), const_cast<char*>(script_path), NULL};
		 */
		char*		args[2] = {const_cast<char*>(path), NULL};
		char**		envp	= (char**)std::calloc(req.getAllHeaders().size() + 1, sizeof(char*));
		if (!envp) {
			close(pipefd[0]);
			close(outfile[1]);
			exit(EXIT_FAILURE);
		}
		int i = 0;
		for (std::map<std::string, std::string>::const_iterator it = req.getAllHeaders().begin(); it != req.getAllHeaders().end(); ++it) {
			std::stringstream env_string;
			// TODO uppercase (*it.first) and replace - by _
			env_string << "HTTP_" << (*it).first << "=" << (*it).second;
			envp[i] = strdup(env_string.str().c_str());
			i++;
		}
		envp[i] = NULL;
		execve(path, args, envp);
		for (int i = 0; envp[i]; ++i)
			std::free(envp[i]);
		std::free(envp);
		std::exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	close(outfile[1]);
	{
		char*		bufwrite = const_cast<char*>(req.getBody().c_str());
		size_t		size	 = req.getBody().size();
		size_t		pos		 = 0;
		std::string output("");
		char		bufread[BUFSIZ + 1];
		bufread[BUFSIZ]		= 0;
		bool write_finished = false;
		bool read_finished	= false;
		set_nonblocking(pipefd[1]);
		set_nonblocking(outfile[0]);
		while (!write_finished || !read_finished) {
			if (!write_finished) {
				ssize_t written = write(pipefd[1], bufwrite + pos, size - pos);
				if (written < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
					write_finished = true;
					close(pipefd[1]);
					pipefd[1] = -1;
				} else if (written >= 0) {
					pos += written;
					write_finished = !(pos < size);
					if (write_finished) {
						close(pipefd[1]);
						pipefd[1] = -1;
					}
				}
			}
			if (!read_finished) {
				ssize_t readn = read(outfile[0], bufread, BUFSIZ);
				if (readn < 0 && !(errno == EAGAIN || errno == EWOULDBLOCK)) {
					read_finished = true;
					close(outfile[0]);
					outfile[0] = -1;
				} else if (readn >= 0) {
					read_finished = readn == 0;
					if (read_finished) {
						close(outfile[0]);
						outfile[0] = -1;
					}
					output.append(bufread, readn);
				}
			}
		}
		/*
		TODO split output at the first blank lineparse the CGI's own headers (Content-Type, Status, etc.) into res, set the body to only the part after the
		separator, and compute Content-Length on that body, not on headers+body.
		*/
		res.setHeader("Content-Length", ft_itoa(output.size()));
		res.setBody(output);
	}
	if (pipefd[1] != -1)
		close(pipefd[1]);
	if (outfile[0] != -1)
		close(outfile[0]);
	int wstatus;
	/*TODO: add a timeout / non-blocking reap with kill-on-timeout; and inspect wstatus — non-zero exit or signal should produce a 502 instead of return true.*/
	waitpid(pid, &wstatus, 0);
	(void)wstatus;
	return (true); // TODO: return value doesn't reflect CGI failure (tied to the wstatus TODO above).
}
