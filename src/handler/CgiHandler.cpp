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

std::string uri_decode(std::string uri) {
	std::string out("");
	for (std::string::iterator it = uri.begin(); it != uri.end(); ++it) {
		if ((*it) == '%') {
			++it;
			if (it == uri.end())
				break;
			if (*it == '%') {
				out.append("%");
				continue;
			}
			char c = 0;
			if ('0' <= *it && *it <= '9')
				c += *it - '0';
			else if ('A' <= *it && *it <= 'F')
				c += *it - 'A' + 10;
			else if ('a' <= *it && *it <= 'f')
				c += *it - 'a' + 10;
			else
				break;
			c *= 16;
			++it;
			if (it == uri.end())
				break;
			if ('0' <= *it && *it <= '9')
				c += *it - '0';
			else if ('A' <= *it && *it <= 'F')
				c += *it - 'A' + 10;
			else if ('a' <= *it && *it <= 'f')
				c += *it - 'a' + 10;
			else
				break;
			out.insert(out.end(), c);
		} else if (*it == '+')
			out.insert(out.end(), ' ');
		else
			out.insert(out.end(), *it);
	}
	return out;
}

bool uri_strip_servername(std::vector<std::string> server_names, std::string& uri) {
	std::string server_name("");
	for (std::vector<std::string>::const_iterator it = server_names.begin(); it != server_names.end(); ++it) {
		if (uri.find((*it), 0) == 0) {
			server_name = *it;
			break;
		}
	}
	if (server_name == "")
		return false;
	uri.replace(0, server_name.size(), "");
	while (uri.at(0) == '/') {
		uri.replace(0, 1, "");
	}
	return true;
}

bool CgiHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc, IServerConfig const* serv) {
	std::string uri = uri_decode(req.getUri());
	if (uri.find("/../") != std::string::npos)
		return false;
	if (!uri_strip_servername(serv->getServerNames(), uri))
		return false;
	std::size_t ext_pos = uri.find_last_of('.');
	if (ext_pos == std::string::npos)
		return false;
	for (std::vector<ILocationConfig::CgiPass>::const_iterator it = loc.getCgiPasses().begin(); it != loc.getCgiPasses().end(); ++it) {
		if (uri.find((*it).extension, ext_pos) == ext_pos && uri.find(loc.getPath()) != std::string::npos) {
			this->cgipass = (*it);
			return true;
		}
	}
	return false;
}

std::string header_tocgi(std::string header) {
	std::string out("");
	for (std::string::const_iterator it = header.begin(); it != header.end(); ++it) {
		if ('a' <= *it && *it <= 'z')
			out.push_back(*it - 'a' + 'A');
		else if (*it == '-')
			out.push_back('_');
		else
			out.push_back(*it);
	}
	return out;
}

bool is_cgi_metavariable(std::string val) {
	if (val == "AUTH_TYPE")
		return true;
	if (val == "CONTENT_LENGTH")
		return true;
	if (val == "CONTENT_TYPE")
		return true;
	if (val == "PATH_INFO")
		return true;
	if (val == "PATH_TRANSLATED")
		return true;
	if (val == "QUERY_STRING")
		return true;
	if (val == "REMOTE_HOST")
		return true;
	if (val == "REMOTE_IDENT")
		return true;
	if (val == "REMOTE_USER")
		return true;
	if (val == "REQUEST_METHOD")
		return true;
	if (val == "SCRIPT_NAME")
		return true;
	if (val == "SERVER_PROTOCOL")
		return true;
	return false;
}
bool is_cgi_request_metavariable(std::string val) {
	if (val == "AUTH_TYPE")
		return true;
	if (val == "CONTENT_LENGTH")
		return true;
	if (val == "CONTENT_TYPE")
		return true;
	if (val == "GATEWAY_INTERFACE")
		return true;
	if (val == "PATH_INFO")
		return true;
	if (val == "PATH_TRANSLATED")
		return true;
	if (val == "QUERY_STRING")
		return true;
	if (val == "REMOTE_ADDR")
		return true;
	if (val == "REMOTE_HOST")
		return true;
	if (val == "REMOTE_IDENT")
		return true;
	if (val == "REMOTE_USER")
		return true;
	if (val == "REQUEST_METHOD")
		return true;
	if (val == "SCRIPT_NAME")
		return true;
	if (val == "SERVER_NAME")
		return true;
	if (val == "SERVER_PORT")
		return true;
	if (val == "SERVER_PROTOCOL")
		return true;
	if (val == "SERVER_SOFTWARE")
		return true;
	return false;
}

bool CgiHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	(void)serv;
	if (!canHandle(req, loc, serv))
		return false;
	int			pipefd[2];
	int			outfile[2];

	/*TODO add the script path
	the script path come from the request
	No traversal ..*/
	std::string script_path = uri_decode(req.getUri());
	uri_strip_servername(serv->getServerNames(), script_path);
	std::stringstream fstring;
	if (!serv->getRootDir().empty())
		fstring << serv->getRootDir().get();
	if (!loc.getRoot().empty())
		fstring << loc.getRoot().get();
	fstring << script_path;
	script_path = fstring.str();

	if (access(script_path.c_str(), F_OK | X_OK) == -1) {
		res.setStatus(500);
		res.setBody("<h1>500 Internal Server Error</h1>");
		return true;
	}
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
		const char* path	= cgipass.interpreter.c_str();
		char*		args[3] = {const_cast<char*>(path), const_cast<char*>(script_path.c_str()), NULL};
		#define SERVER_HEADER_LEN 6
		/*SERVER_HEADER are headers built in by the server*/
		char**		envp	= (char**)std::calloc(req.getAllHeaders().size() + SERVER_HEADER_LEN + 1, sizeof(char*));
		if (!envp) {
			close(pipefd[0]);
			close(outfile[1]);
			exit(EXIT_FAILURE);
		}
		envp[0] = strdup("GATEWAY_INTERFACE=CGI/1.1");
		envp[1] = strdup("REMOTE_ADDR="); //TODO Pass the IP Address of the client
		envp[2] = strdup("REMOTE_HOST="); //TODO GET the client remote host
		envp[3] = strdup("SERVER_NAME=");//TODO local server name
		envp[4] = strdup("SERVER_PORT=");//TODO server port
		envp[5] = strdup("SERVER_SOFTWARE=webserv");
		int i = SERVER_HEADER_LEN;
		for (std::map<std::string, std::string>::const_iterator it = req.getAllHeaders().begin(); it != req.getAllHeaders().end(); ++it) {
			std::stringstream env_string;
			if (!is_cgi_metavariable(header_tocgi((*it).first)))
				env_string << "HTTP_";
			env_string << header_tocgi((*it).first) << "=" << (*it).second;
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
