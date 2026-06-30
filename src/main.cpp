/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:54 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/08 22:38:25 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"
#include "config/ConfigError.hpp"
#include "config/ConfigParser.hpp"
#include "server/WebServer.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

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
	IWebServer* webServer = new WebServer();
	try {
		webServer->init(config);
		webServer->run();
	} catch (const WebServerError& e) {
		std::cerr << "Failed to launch config with error :" << e.what() << std::endl;
	} catch (const std::runtime_error& e) {
		std::cerr << "Failed to launch config with error:" << e.what() << std::endl;
	}
	delete webServer;
}
