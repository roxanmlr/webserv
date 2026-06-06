/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:52 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/06 11:52:47 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "../config/IServerConfig.hpp"
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
	enum ReadStatus { READ_OK, READ_CLOSED, READ_AGAIN, READ_ERROR, READ_OVERFLOW };
	enum ReadStatus read_status;
	std::string		read_buffer;
	enum WriteStatus {
		WRITE_OK,
		WRITE_AGAIN,
		WRITE_DONE,
		WRITE_ERROR,
	};
	enum WriteStatus write_status;
	std::size_t		 write_pos;
	std::string		 write_buffer;

public:
	Client();
	Client(int fd, IServerConfig const* serv);
	~Client();
	Client(Client const& other);
	Client& operator=(Client const& other);
	int		getFd() const;
	State	getState() const;
	time_t	getLastActivity() const;
	void	onReadable();
	void	setWriteBuffer(std::string const& write_buffer);
	void	onWritable();
	bool	wantsRead() const;
	bool	wantsWrite() const;
	bool	shouldClose() const;
};
#endif
