/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:36 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/06 11:57:28 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer() : listen_map(), _shouldClose(false) {
}

WebServer::~WebServer() {
	stop();
}

WebServer::WebServer(WebServer const& other) {
	if (this == &other)
		return;
	*this = other;
}

WebServer& WebServer::operator=(WebServer const& other) {
	if (this == &other)
		return *this;
	this->listen_map   = other.listen_map;
	this->_shouldClose = other._shouldClose;
	return *this;
}

void	WebServer::shouldClose(){
	this->_shouldClose = true;
}

int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void WebServer::init(const IConfig& config) {
	const std::vector<IServerConfig*> servers = config.getServers();
	for (std::vector<IServerConfig*>::const_iterator it_server = servers.begin(); it_server != servers.end(); ++it_server) {
		for (std::vector<IServerConfig::ListenAddress>::const_iterator it_address = (*it_server)->getListenAddresses().begin();
			 it_address != (*it_server)->getListenAddresses().end(); it_address++) {
			int				   port = (*it_address).port;
			std::string		   host = (*it_address).host;
			struct sockaddr_in addr;
			int				   opt = 1;
			int				   fd  = socket(AF_INET, SOCK_STREAM, 0);
			if (fd == -1)
				throw WebServerError("socket");
			if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
				close(fd);
				throw WebServerError("setsockopt");
			}
			if (set_nonblocking(fd) == -1) {
				close(fd);
				throw WebServerError("fcntl");
			}
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port	= htons(port);
			if (host == "0.0.0.0" || host == "*" || host.empty()) {
				addr.sin_addr.s_addr = htonl(INADDR_ANY);
			} else {
				int result = inet_pton(AF_INET, host.c_str(), &(addr.sin_addr));
				if (result < 0) {
					close(fd);
					throw WebServerError("Famille d'addresse non valide pour : " + host);
				} else if (result == 0) {
					close(fd);
					throw WebServerError("IP invalide : " + host);
				}
			}
			if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
				close(fd);
				// throw WebServerError("bind sur port " + std::to_string(port));
				throw WebServerError("bind");
			}
			if (listen(fd, BACKLOG) == -1) {
				close(fd);
				throw WebServerError("listen");
			}
			Logger logger;
			logger.log(Logger::INFO, "New Server");
			listen_map[fd] = (struct ListenConfig){.fd = fd, .host = host, .port = port, .serv = (*it_server)};
		}
	}
}

void WebServer::addClient(int epoll_fd, int server_fd) {
	for (;;) {
		Logger logger;
		int	   client_fd = accept(server_fd, NULL, NULL);
		if (client_fd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break; // Plus de client à accepter fin de boucle
			// Logger.error("accept failed")
			break;
		}
		if (client_map.count(client_fd) > 0)
			break;
		if (set_nonblocking(client_fd) == -1) {
			// Logger.error("fcntl failed")
			close(client_fd);
			continue;
		}
		struct epoll_event ev_client;
		std::memset(&ev_client, 0, sizeof(ev_client));
		ev_client.events  = EPOLLIN;
		ev_client.data.fd = client_fd;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_client) == -1) {
			// Logger.error("epoll_ctl failed")
			close(client_fd);
			continue;
		}
		logger.log(Logger::INFO, "Client connected");
		client_map[client_fd]	 = new Client(client_fd, listen_map[server_fd].serv);
		std::string defaultWrite = "HTTP/1.1 200 OK\r\n"
								   "Content-Type: text/plain\r\n"
								   "Content-Length: 12\r\n"
								   "Connection: close\r\n"
								   "\r\n"
								   "Hello World\n";
		(client_map[client_fd])->setWriteBuffer(defaultWrite);
	}
}

void WebServer::serveClient(int epoll_fd, struct epoll_event events[MAX_EVENTS], int event_pos, int client_fd) {
	IClient* client = client_map[client_fd];
	if (events[event_pos].events & EPOLLIN) {
		client->onReadable();
	}
	if (events[event_pos].events & EPOLLOUT) {
		client->onWritable();
	}

	if (client->shouldClose()) {
		close(client_fd);
		delete client_map[client_fd];
		client_map.erase(client_fd);
	} else {
		struct epoll_event ev;
		ev.data.fd = client->getFd();
		ev.events  = 0;
		if (client->wantsRead())
			ev.events |= EPOLLIN;
		if (client->wantsWrite())
			ev.events |= EPOLLOUT;
		epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->getFd(), &ev);
	}
}

void WebServer::run() {
	int epoll_fd = -1;
	try {
		struct epoll_event events[MAX_EVENTS];
		std::signal(SIGPIPE, SIG_IGN);
		epoll_fd = epoll_create(1);
		if (epoll_fd == -1)
			throw WebServerError("epoll_create");
		for (std::map<int, WebServer::ListenConfig>::iterator it = listen_map.begin(); it != listen_map.end(); ++it) {
			struct epoll_event ev;
			std::memset(&ev, 0, sizeof(ev));
			ev.events  = EPOLLIN;
			ev.data.fd = (*it).first;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, (*it).first, &ev) == -1) {
				close(epoll_fd);
				throw WebServerError("epoll_ctl server");
			}
		}
		for (;;) {
			int n_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
			if (n_events == -1) {
				if (errno == EINTR)
					continue;
				throw WebServerError("epoll_wait");
			}
			if (_shouldClose)
				break;
			for (int i = 0; i < n_events; ++i) {
				int	 fd			  = events[i].data.fd;
				bool is_server_fd = listen_map.count(fd) > 0;
				if (is_server_fd)
					addClient(epoll_fd, fd);
				else
					serveClient(epoll_fd, events, i, fd);
			}
		}
		this->stop();
		close(epoll_fd);
	} catch (...) {
		if (epoll_fd != -1)
			close(epoll_fd);
		std::cerr << "server error" << std::endl;
	}
}

void WebServer::stop() {
	for (std::map<int, WebServer::ListenConfig>::iterator it = listen_map.begin(); it != listen_map.end(); ++it) {
		close((*it).first);
	}
	for (std::map<int, IClient*>::iterator it = client_map.begin(); it != client_map.end(); ++it) {
		close(it->first);
		delete it->second;
	}
	listen_map.clear();
	client_map.clear();
}
