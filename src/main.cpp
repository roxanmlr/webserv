/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:54 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:54 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/Config.hpp"
#include "config/ConfigError.hpp"
#include "config/ConfigParser.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cerr << "Usage: " << argv[1] << "[config_file.conf]" << std::endl;
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
}
