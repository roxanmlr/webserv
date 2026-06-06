/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:48 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/05 23:09:44 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>
#include <string>

class Logger {
public:
	enum Level { DEBUG, INFO, WARN, ERROR };
	Logger();
	~Logger();
	Logger(Logger const& other);
	Logger& operator=(Logger const& other);
	void	log(Level level, const std::string& msg);
};
#endif
