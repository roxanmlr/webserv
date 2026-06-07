/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:54 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/06 12:13:04 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"
#include "config/ConfigError.hpp"
#include "config/ConfigParser.hpp"
#include "server/WebServer.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <unistd.h>

static IWebServer* webServer;

void			   shutdownServer(int s) {
	  (void)s;
	  if (!webServer)
		  return;
	  webServer->shouldClose();
}

int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << "[config_file.conf]" << std::endl;
		return EXIT_FAILURE;
	}
	Config config;
	if (argc == 1) {
		std::ifstream f("config/default.conf");
		if (f.good())
			f >> config;
	} else {
		std::ifstream f(argv[1]);
		if (f.good())
			f >> config;
	}
	std::cout << config;
	if (!config.good()) {
		std::cerr << "Incorrect configuration " << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cerr << "To shutdown the server properly:\n\tkill -s SIGTERM " << getpid() << std::endl;
	webServer = new WebServer();
	signal(SIGTERM, shutdownServer);
	try {
		webServer->init(config);
	} catch (WebServerError& e) {
		std::cerr << "Failed to launch config with error :" << e.what() << std::endl;
		delete webServer;
		exit(EXIT_FAILURE);
	}
	webServer->run();
	delete webServer;
}
