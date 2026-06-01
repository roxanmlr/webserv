/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:18 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/28 00:21:53 by lmilando         ###   ########.fr       */
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
#include <unistd.h>

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

public:
	WebServer();
	~WebServer();
	WebServer(WebServer const& other);
	WebServer& operator=(WebServer const& other);
	void	   init(const IConfig& config);
	void	   run();
	void	   stop();
};
#endif
