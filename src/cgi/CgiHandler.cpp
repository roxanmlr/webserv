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

static void		   parse_cgi_response(const std::string& output, IHttpResponse& res);
static std::string header_tocgi(const std::string& header);
static char**	   build_cgi_env(const IHttpRequest& req, IServerConfig const* serv, const std::string& script_name);
static void		   free_envp(char** envp);

CgiHandler::CgiHandler()
	: cgipass(), pid(-1), state(INIT), output(""), write_finished(false), bufwrite(NULL), bufwrite_pos(0), bufwrite_size(0), read_finished(false), serv(NULL),
	  s_time(0) {
	pipefd[0]  = -1;
	pipefd[1]  = -1;
	outfile[0] = -1;
	outfile[1] = -1;
}

CgiHandler::~CgiHandler() {
	free(bufwrite);
}

CgiHandler::CgiHandler(const CgiHandler& base)
	: cgipass(base.cgipass), pid(base.pid), state(base.state), output(base.output), write_finished(base.write_finished), bufwrite(base.bufwrite),
	  bufwrite_pos(base.bufwrite_pos), bufwrite_size(base.bufwrite_size), read_finished(base.read_finished), serv(base.serv), s_time(base.s_time) {
	pipefd[0]  = base.pipefd[0];
	pipefd[1]  = base.pipefd[1];
	outfile[0] = base.outfile[0];
	outfile[1] = base.outfile[1];
}

CgiHandler& CgiHandler::operator=(const CgiHandler& base) {
	if (this == &base)
		return *this;
	this->cgipass = base.cgipass;
	this->state	  = base.state;
	this->output  = base.output;
	this->pid	  = base.pid;
	pipefd[0]	  = base.pipefd[0];
	pipefd[1]	  = base.pipefd[1];
	outfile[0]	  = base.outfile[0];
	outfile[1]	  = base.outfile[1];
	return (*this);
}

int CgiHandler::getInputFd() const {
	return pipefd[1];
}
int CgiHandler::getOutputFd() const {
	return outfile[0];
}
bool CgiHandler::isFinished() {
	std::cerr << "Test fin CGI\n";
	hasTimeOut();
	if (state == ERROR || state == FINISHED || state == TIMEOUT) {
		std::cerr << "CGI deja terminé\n";
		return true;
	}

	if (state == PROCESSING) {
		int wstatus;
		int child_pid = waitpid(pid, &wstatus, WNOHANG);
		if (child_pid == 0) {
			std::cerr << "Client runninng\n";
			return false;
		}
		if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus) != 0) {
			std::cerr << "GATEWAY Non null exit status" << std::endl;
			state = ERROR;
			pid	  = -1;
			return true;
		}
		std::cerr << "Client terminé\n";
		state = PROCESS_END;
	}
	if (read_finished && write_finished) {
		state = FINISHED;
		std::cerr << "CGI terminé\n";
		return true;
	}
	std::cerr << "CGI continue\n";
	return false;
}
void CgiHandler::closeFdOnError() {
	if (outfile[0] != -1) {
		close(outfile[0]);
		outfile[0] = -1;
		std::cerr << "Fermeture de la sortie outfile[0] du CGI\n";
	}
	if (pipefd[1] != -1) {
		close(pipefd[1]);
		pipefd[1] = -1;
		std::cerr << "Fermeture de l'entree pipefd[1] du CGI\n";
	}
	if (pid != -1) {
		kill(pid, SIGKILL);
		pid = -1;
		std::cerr << "Le Client a été SIGKILL\n";
	}
}
void CgiHandler::hasTimeOut() {
	if (!serv || state == ERROR || state == FINISHED)
		return;
	if ((!serv->getTimeOut().empty() && static_cast<size_t>(difftime(time(NULL), s_time)) >= serv->getTimeOut().get()) ||
		(serv->getTimeOut().empty() && difftime(time(NULL), s_time) >= TIME_OUT_CGI)) {
		closeFdOnError();
		std::cerr << "CGI TimeOut\n";
		state = TIMEOUT;
	}
}
bool CgiHandler::onOutput() {
	std::cerr << "Host reading from client\n";
	if (read_finished) {
		std::cerr << "Host reading deja terminé\n";
		return true;
	}
	char bufread[BUFSIZ + 1];
	bufread[BUFSIZ] = 0;
	ssize_t readn	= read(outfile[0], bufread, BUFSIZ);
	/*if (readn < 0) {
		read_finished = true;
		state		  = ERROR;
		std::cerr << "Host reading terminé suite à erreur\n";
		closeFdOnError();
	} else*/
	if (readn >= 0) {
		read_finished = readn == 0;
		if (read_finished) {
			close(outfile[0]);
			outfile[0] = -1;
			std::cerr << "Host reading terminé proprement\n";
		}
		output.append(bufread, readn);
	}
	return read_finished;
}
bool CgiHandler::onInput() {
	std::cerr << "Host writing to client\n";
	if (write_finished) {
		std::cerr << "Host writing deja terminé\n";
		return true;
	}
	ssize_t written = write(pipefd[1], bufwrite + bufwrite_pos, bufwrite_size - bufwrite_pos);
	/*if (written < 0) {
		write_finished = true;
		state		   = ERROR;
		std::cerr << "Host writing terminé suite à erreur\n";
		closeFdOnError();
	} else*/
	if (written >= 0) {
		bufwrite_pos += written;
		write_finished = !(bufwrite_pos < bufwrite_size);
		if (write_finished) {
			close(pipefd[1]);
			pipefd[1] = -1;
			std::cerr << "Host writing terminé proprement\n";
		}
	}
	if (isFinished()) {
		std::cerr << "Programme terminé\n";
		write_finished = true;
		close(pipefd[1]);
		pipefd[1] = -1;
		std::cerr << "Fermeture de Host writing\n";
		return true;
	}
	std::cerr << "Host writing continue";
	return false;
}

void CgiHandler::fillResponse(IHttpResponse& res) {
	if (!isFinished())
		return;
	if (pipefd[1] != -1)
		close(pipefd[1]);
	if (outfile[0] != -1)
		close(outfile[0]);

	if (state == TIMEOUT) {
		std::cerr << "Fill timeout error\n";
		HttpResponse::applyErrorPage(res, 504, serv);
		return;
	}
	if (state == ERROR) {
		std::cerr << "Fill state error\n";
		HttpResponse::applyErrorPage(res, 502, serv);
		return;
	}
	parse_cgi_response(output, res);
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

bool CgiHandler::canHandle(const IHttpRequest& req, const ILocationConfig& loc) {
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
		if ((ext_pos + (*it).extension.size()) == uri.size() && uri.find((*it).extension, ext_pos) == ext_pos && uri.find(loc.getPath(), 0) == 0) {
			this->cgipass = (*it);
			return true;
		}
	}
	return false;
}

bool CgiHandler::handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* server) {
	serv					= server;
	std::string script_path = uri_decode(req.getUri());
	std::size_t q			= script_path.find('?');
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
		std::cerr << "Script error : " << script_path;
		std::cerr << "\n\tF_OK:" << access(script_path.c_str(), F_OK) << std::endl;
		std::cerr << "\n\tX_OK:" << access(script_path.c_str(), X_OK) << std::endl;
		state = ERROR;
		pid	  = -1;
		closeFdOnError();
		(void)res;
		return false;
	}
	std::string cgi_script_name = req.getUri();
	std::size_t qs_pos			= cgi_script_name.find('?');
	if (qs_pos != std::string::npos)
		cgi_script_name = cgi_script_name.substr(0, qs_pos);

	if (pipe(pipefd) == -1) {
		state = ERROR;
		throw WebServerError("pipe failure");
	}
	if (pipe(outfile) == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		state = ERROR;
		throw WebServerError("pipe failure");
	}
	int pid = fork();
	if (pid == -1) {
		close(pipefd[0]);
		close(pipefd[1]);
		state = ERROR;
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
	state  = PROCESSING;
	s_time = time(NULL);
	close(pipefd[0]);
	pipefd[0] = -1;
	close(outfile[1]);
	outfile[1] = -1;
	set_nonblocking(pipefd[1]);
	set_nonblocking(outfile[0]);
	bufwrite	   = strdup(const_cast<char*>(req.getBody().c_str()));
	bufwrite_size  = req.getBody().size();
	bufwrite_pos   = 0;
	write_finished = false;
	read_finished  = false;
	std::cerr << "CGI Lancé\n";
	return true;
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
		} else if (name != "Content-Length") {
			res.setHeader(name, value);
		}
	}

	res.setStatus(status);
	res.setBody(body);
}

static void free_envp(char** envp) {
	for (int i = 0; envp[i]; ++i)
		std::free(envp[i]);
	std::free(envp);
}

static char** build_cgi_env(const IHttpRequest& req, IServerConfig const* serv, const std::string& script_name) {
	std::vector<std::string> envvec;

	const std::string&		 raw_uri = req.getUri();
	std::string				 query_string;
	std::size_t				 q_pos = raw_uri.find('?');
	if (q_pos != std::string::npos)
		query_string = raw_uri.substr(q_pos + 1);

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