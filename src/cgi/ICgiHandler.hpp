/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ICgiHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:39 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:39 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ICGI_HANDLER_HPP
#define ICGI_HANDLER_HPP
#include "../handler/IRequestHandler.hpp"

class ICgiHandler : public IRequestHandler {
public:
	virtual ~ICgiHandler() {
	}
	virtual int	 getInputFd() const	 = 0; // pipe vers STDIN du CGI
	virtual int	 getOutputFd() const = 0; // pipe vers STDOUT du CGI
	virtual bool isFinished() const	 = 0;
	virtual void onInputWritable()	 = 0; // appelé sur POLLOUT du pipe stdin
	virtual void onOutputWritable()	 = 0; // appelé sur POLLOUT du pipe stdout
};
#endif
