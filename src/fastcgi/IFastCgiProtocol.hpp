/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IFastCgiProtocol.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:51 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:51 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IFAST_CGI_PROTOCOL_HPP
#define IFAST_CGI_PROTOCOL_HPP
#include <map>
#include <string>
#include <vector>

class IFastCgiProtocol {
public:
	struct Record {
		unsigned int type;
		unsigned int requestId;
		std::string	 content;
	};
	virtual ~IFastCgiProtocol() {
	}
	virtual std::string			encodeBeginRequest(unsigned int requestId)										 = 0;
	virtual std::string			encodeParams(unsigned int requestId, std::map<std::string, std::string>& params) = 0;
	virtual std::string			encodeStdin(unsigned int requestId, const std::string& body)					 = 0;
	virtual std::vector<Record> decode(std::string& buffer)														 = 0;
};
#endif
