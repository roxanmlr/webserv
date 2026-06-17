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
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sstream>

const int CgiHandler::TIMEOUT_SEC = 30;

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
	(void)serv;
	std::string uri = uri_decode(req.getUri());
	if (uri.find("/../") != std::string::npos)
		return false;
	// Strip query string before extension matching
	std::size_t q = uri.find('?');
	if (q != std::string::npos)
		uri = uri.substr(0, q);
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

static std::string header_tocgi(const std::string& header) {
	std::string out;
	out.reserve(header.size());
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

static void parse_cgi_response(const std::string& output, IHttpResponse& res) {
	std::size_t sep_pos = output.find("\r\n\r\n"); // Find 2 blank lines with carriage \r
	std::size_t sep_len = 4;
	if (sep_pos == std::string::npos) {
		sep_pos = output.find("\n\n"); // Find 2 blank lines without carriage
		sep_len = 2;
	}
	std::string body;
	std::string header_section;
	if (sep_pos == std::string::npos) {
		body = output;
	} else {
		header_section = output.substr(0, sep_pos);
		body		   = output.substr(sep_pos + sep_len);
	}

	int				   status = 200;

	// Parse CGI headers line by line
	std::istringstream ss(header_section);
	std::string		   line;
	while (std::getline(ss, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			continue;
		std::size_t colon = line.find(':');
		if (colon == std::string::npos)
			continue;
		std::string name  = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		std::size_t first = value.find_first_not_of(" \t");
		value			  = (first != std::string::npos) ? value.substr(first) : "";

		if (name == "Status") {
			// "Status: 404 Not Found" → extract the numeric code
			std::istringstream code_ss(value);
			code_ss >> status;
		} else {
			res.setHeader(name, value);
		}
	}

	res.setStatus(status);
	res.setHeader("Content-Length", ft_itoa(body.size()));
	res.setBody(body);
}

static void free_envp(char** envp) {
	for (int i = 0; envp[i]; ++i)
		std::free(envp[i]);
	std::free(envp);
}

static char** build_cgi_env(const IHttpRequest& req, IServerConfig const* serv, const std::string& script_name) {
	std::vector<std::string> envvec;

	// Extract query string from URI (raw, not decoded)
	const std::string&		 raw_uri = req.getUri();
	std::string				 query_string;
	std::size_t				 q_pos = raw_uri.find('?');
	if (q_pos != std::string::npos)
		query_string = raw_uri.substr(q_pos + 1);

	// SERVER_NAME and SERVER_PORT: prefer Host header, fall back to config
	std::string host		= req.getHeader("Host");
	std::string server_name = host;
	std::string server_port;
	std::size_t colon = host.find(':');
	if (colon != std::string::npos) {
		server_name = host.substr(0, colon);
		server_port = host.substr(colon + 1);
	}
	if (server_name.empty() && !serv->getServerNames().empty())
		server_name = serv->getServerNames()[0];
	if (server_port.empty() && !serv->getListenAddresses().empty()) {
		std::ostringstream ss;
		ss << serv->getListenAddresses()[0].port;
		server_port = ss.str();
	}

	// All required CGI/1.1 metavariables (RFC 3875 §4.1)
	envvec.push_back("AUTH_TYPE=");
	envvec.push_back("CONTENT_LENGTH=" + req.getHeader("Content-Length"));
	envvec.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
	envvec.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envvec.push_back("PATH_INFO=");
	envvec.push_back("PATH_TRANSLATED=");
	envvec.push_back("QUERY_STRING=" + query_string);
	envvec.push_back("REMOTE_ADDR=");
	envvec.push_back("REMOTE_HOST=");
	envvec.push_back("REQUEST_METHOD=" + req.getMethod());
	envvec.push_back("SCRIPT_NAME=" + script_name);
	envvec.push_back("SERVER_NAME=" + server_name);
	envvec.push_back("SERVER_PORT=" + server_port);
	envvec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envvec.push_back("SERVER_SOFTWARE=webserv");

	// Protocol-specific HTTP headers as HTTP_* (skip CONTENT_TYPE and CONTENT_LENGTH)
	for (std::map<std::string, std::string>::const_iterator it = req.getAllHeaders().begin(); it != req.getAllHeaders().end(); ++it) {
		std::string cgi_name = header_tocgi(it->first);
		if (cgi_name == "CONTENT_TYPE" || cgi_name == "CONTENT_LENGTH")
			continue;
		envvec.push_back("HTTP_" + cgi_name + "=" + it->second);
	}

	char** envp = (char**)std::calloc(envvec.size() + 1, sizeof(char*));
	if (!envp)
		return NULL;
	for (std::size_t i = 0; i < envvec.size(); ++i) {
		envp[i] = strdup(envvec[i].c_str());
		if (!envp[i]) {
			for (std::size_t j = 0; j < i; ++j)
				std::free(envp[j]);
			std::free(envp);
			return NULL;
		}
	}
	envp[envvec.size()] = NULL;
	return envp;
}

bool CgiHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv) {
	int			pipefd[2];
	int			outfile[2];

	std::string script_path = uri_decode(req.getUri());
	std::size_t q = script_path.find('?');
	if (q != std::string::npos)
		script_path = script_path.substr(0, q);
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
	std::string cgi_script_name = req.getUri();
	std::size_t qs_pos			= cgi_script_name.find('?');
	if (qs_pos != std::string::npos)
		cgi_script_name = cgi_script_name.substr(0, qs_pos);

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
		char**		envp	= build_cgi_env(req, serv, cgi_script_name);
		if (!envp) {
			close(pipefd[0]);
			close(outfile[1]);
			std::exit(EXIT_FAILURE);
		}
		execve(path, args, envp);
		free_envp(envp);
		std::exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	close(outfile[1]);

	bool		timed_out = false;
	std::string output;
	{
		char*  bufwrite = const_cast<char*>(req.getBody().c_str());
		size_t size		= req.getBody().size();
		size_t pos		= 0;
		char   bufread[BUFSIZ + 1];
		bufread[BUFSIZ]		= 0;
		bool write_finished = false;
		bool read_finished	= false;
		set_nonblocking(pipefd[1]);
		set_nonblocking(outfile[0]);
		time_t deadline = time(NULL) + TIMEOUT_SEC;
		while (!write_finished || !read_finished) {
			if (time(NULL) >= deadline) {
				kill(pid, SIGKILL);
				timed_out = true;
				break;
			}
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
	}

	if (pipefd[1] != -1)
		close(pipefd[1]);
	if (outfile[0] != -1)
		close(outfile[0]);

	int wstatus;
	waitpid(pid, &wstatus, 0);

	if (timed_out) {
		res.setStatus(504);
		res.setBody("<h1>504 Gateway Timeout</h1>");
		return true;
	}
	if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) {
		res.setStatus(502);
		res.setBody("<h1>502 Bad Gateway</h1>");
		return true;
	}
	parse_cgi_response(output, res);
	return true;
}
