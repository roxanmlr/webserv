/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigError.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:01 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:02 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIG_ERROR_HPP
#define CONFIG_ERROR_HPP
#include <exception>
#include <iostream>
#include <string>

class ConfigError : public std::exception {
private:
	std::string message;

public:
	ConfigError();
	ConfigError(const std::string& message);
	ConfigError(ConfigError const& other);
	ConfigError& operator=(ConfigError const& other);
	~ConfigError() throw();
	const char* what() const throw();
};
#endif
