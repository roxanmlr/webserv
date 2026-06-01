/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:52 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/01 20:23:53 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "../config/IServerConfig.hpp"
#include "IClient.hpp"

class Client : public IClient {
private:
	int					 fd;
	enum IClient::State	 state;
	IServerConfig const* serv;
	time_t				lastActivity;

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
	void	onWritable();
	bool	wantsRead() const;
	bool	wantsWrite() const;
	bool	shouldClose() const;
};
#endif
