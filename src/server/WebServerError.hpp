/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServerError.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:13 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:49:14 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef WEBSERVER_ERROR_HPP
#define WEBSERVER_ERROR_HPP
#include <exception>
#include <iostream>
#include <string>

class WebServerError : public std::exception {
private:
	std::string message;

public:
	WebServerError();
	WebServerError(const std::string& message);
	WebServerError(WebServerError const& other);
	WebServerError& operator=(WebServerError const& other);
	~WebServerError() throw();
	const char* what() const throw();
};
#endif
