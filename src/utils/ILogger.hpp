/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ILogger.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:48 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:48 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ILOGGER_HPP
#define ILOGGER_HPP
#include <string>

class ILogger {
public:
	enum Level { DEBUG, INFO, WARN, ERROR };

	virtual ~ILogger() {
	}
	virtual void log(Level level, const std::string& msg) = 0;
};
#endif
