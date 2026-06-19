/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:36 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/19 01:48:00 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer() : listen_map(), _shouldClose(false) {
}

WebServer::~WebServer() {
	stop();
}

void WebServer::shouldClose() {
	this->_shouldClose = true;
}

void WebServer::init(const IConfig& config) {
	{
		struct sockaddr_un addr;
		shutdown_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
		if (shutdown_fd == -1)
			throw WebServerError("AF_UNIX socket");
		memset(&addr, 0, sizeof(addr));
		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, CLOSE_SOCKET_PATH, sizeof(addr.sun_path) - 1);
		unlink(CLOSE_SOCKET_PATH);
		if (bind(shutdown_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
			close(shutdown_fd);
			throw WebServerError("bind AF_UNIX");
		}
		if (set_nonblocking(shutdown_fd) == -1) {
			close(shutdown_fd);
			throw WebServerError("fcntl");
		}
	}
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
			if (listen(fd, SOMAXCONN) == -1) {
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
		client_map[client_fd] = new Client(client_fd, listen_map[server_fd].serv);
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
		std::memset(&ev, 0, sizeof(ev));
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
		{
			struct epoll_event ev;
			std::memset(&ev, 0, sizeof(ev));
			ev.events  = EPOLLIN;
			ev.data.fd = shutdown_fd;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, shutdown_fd, &ev)) {
				close(epoll_fd);
				throw WebServerError("epoll_ctl server/shutdown fd");
			}
		}
		std::cerr << "Shutdown properly with : \n\t echo \"stop\" | nc -uU " << CLOSE_SOCKET_PATH << std::endl;
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
				else if (fd == shutdown_fd) {
					char	buffer[48];
					ssize_t nread = recv(shutdown_fd, buffer, sizeof(buffer) - 1, 0);
					if (nread >= 0) {
						std::cerr << "Shutting down the server" << std::endl;
						this->shouldClose();
						break;
					} else if (nread == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
						std::cerr << "hi" << std::endl;
						// throw WebServerError("recv");
					}
				} else
					serveClient(epoll_fd, events, i, fd);
			}
			for (std::map<int, IClient*>::iterator it = client_map.begin(); it != client_map.end();) {
				IClient* c = it->second;
				if (c && c->isTimeOut()) {
					c->onWritable();
					close(it->first);
					delete c;
					std::map<int, IClient*>::iterator to_del = it;
					++it;
					client_map.erase(to_del);
					continue;
				}
				++it;
			}
		}
		this->stop();
		close(shutdown_fd);
		unlink(CLOSE_SOCKET_PATH);
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
