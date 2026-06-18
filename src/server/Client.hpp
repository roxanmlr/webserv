/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:52 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/19 01:17:30 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "../config/IServerConfig.hpp"
#include "../handler/CgiHandler.hpp"
#include "../handler/StaticFileHandler.hpp"
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "../http/IHttpRequest.hpp"
#include "IClient.hpp"
#include <ctime>
#include <errno.h>
#include <sys/socket.h>

class Client : public IClient {
private:
	int					 fd;
	enum IClient::State	 state;
	IServerConfig const* serv;
	time_t				 lastActivity;
	enum ReadStatus { READ_OK, READ_CLOSED, READ_AGAIN, READ_ERROR, READ_OVERFLOW, READ_TIMEOUT };
	enum ReadStatus read_status;
	std::string		read_buffer;
	enum WriteStatus {
		WRITE_NOT_START,
		WRITE_READY,
		WRITE_OK,
		WRITE_AGAIN,
		WRITE_DONE,
		WRITE_ERROR,
	};
	enum WriteStatus write_status;
	std::size_t		 write_pos;
	std::string		 write_buffer;
	HttpResponse	 response;

	HttpRequest		 _request;

public:
	Client();
	Client(int fd, IServerConfig const* serv);
	~Client();
	Client(Client const& other);
	Client& operator=(Client const& other);
	int		getFd() const;
	State	getState() const;
	time_t	getLastActivity() const;
	bool	isTimeOut();
	void	onReadable();
	void	setWriteBuffer(std::string const& write_buffer);
	void	onWritable();
	bool	wantsRead() const;
	bool	wantsWrite() const;
	bool	shouldClose() const;
};
#endif
