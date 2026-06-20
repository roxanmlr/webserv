/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IClient.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:49:22 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/20 22:36:45 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ICLIENT_HPP
#define ICLIENT_HPP
#include <ctime>

class IClient {
public:
	enum State { INIT, READING_HEADERS, READING_BODY, PROCESSING, WRITING, CLOSING, PARSE_ERROR, TIMEOUT };
	virtual ~IClient() {
	}
	virtual int	   getFd() const								   = 0;
	virtual State  getState() const								   = 0;
	virtual time_t getLastActivity() const						   = 0;
	virtual bool   isTimeOut()									   = 0;
	virtual void   onReadable()									   = 0;
	virtual void   setWriteBuffer(std::string const& write_buffer) = 0;
	virtual void   onWritable()									   = 0;
	virtual bool   wantsRead() const							   = 0;
	virtual bool   wantsWrite() const							   = 0;
	virtual bool   shouldClose() const							   = 0;
	virtual bool   shouldBeHandleByCGI()						   = 0;
	virtual void   handleByCGI()								   = 0;
	virtual void   getCgiFd(int& input, int& output)			   = 0;
	virtual bool   onCgiInput()									   = 0;
	virtual bool   onCgiOutput()								   = 0;
	virtual bool   isCgiFinished()								   = 0;
};
#endif
