/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IHttpResponse.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:02 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:03 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef IHTTP_RESPONSE_HPP
#define IHTTP_RESPONSE_HPP

#include <string>

class IHttpResponse {
public:
	virtual ~IHttpResponse() {
	}
	virtual void		setStatus(int code)											 = 0;
	virtual void		setHeader(const std::string& name, const std::string& value) = 0;
	virtual void		setBody(const std::string& body)							 = 0;
	virtual std::string serialize() const											 = 0; // bytes prêts à être envoy&s
};
#endif
