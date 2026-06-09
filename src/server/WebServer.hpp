/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:18 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/09 15:28:45 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP
#include "Client.hpp"
#include "IWebServer.hpp"
#include "WebServerError.hpp"
#include <arpa/inet.h>
#include <csignal>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <map>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define MAX_EVENTS		  8096
#define CLOSE_SOCKET_PATH "/tmp/close_webserver.sock"

class WebServer : public IWebServer {
private:
	struct ListenConfig {
		int			   fd;
		std::string	   host;
		int			   port;
		IServerConfig* serv;
	};
	std::map<int, struct ListenConfig> listen_map;
	std::map<int, IClient*>			   client_map;
	bool							   _shouldClose;
	int								   shutdown_fd;
	void							   addClient(int epoll_fd, int server_fd);
	void							   serveClient(int epoll_fd, struct epoll_event events[MAX_EVENTS], int event_pos, int client_fd);

public:
	WebServer();
	~WebServer();
	WebServer(WebServer const& other);
	WebServer& operator=(WebServer const& other);
	void	   init(const IConfig& config);
	void	   run();
	void	   stop();
	void	   shouldClose();
};
#endif
