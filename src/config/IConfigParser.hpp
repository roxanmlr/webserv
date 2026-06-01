/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:03 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:47:04 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ICONFIG_PARSER_HPP
#define ICONFIG_PARSER_HPP
#include "IConfig.hpp"
#include <string>

class IConfigParser {
public:
	virtual ~IConfigParser() {
	}
	virtual IConfig* parseDefault(void)				= 0;
	virtual IConfig* parse(const std::string& path) = 0;
};
#endif
