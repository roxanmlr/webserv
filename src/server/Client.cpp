/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:45 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/01 20:23:46 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : fd(-1), state(INIT), serv(NULL), lastActivity() {
}

Client::Client(int fd, IServerConfig const* serv): fd(fd), state(INIT), serv(serv), lastActivity() {
}

Client::~Client() {
}

Client::Client(Client const& other): fd(other.fd), state(other.state), serv(other.serv), lastActivity(other.lastActivity) {
}

Client& Client::operator=(Client const& other) {
	if (this == &other)
		return *this;
	this->fd = other.fd;
	this->state = other.state;
	this->serv = other.serv;
	this->lastActivity = other.lastActivity;
	return *this;
}

int Client::getFd() const {
	return fd;
}

IClient::State Client::getState() const {
	return state;
}

time_t Client::getLastActivity() const {
	return lastActivity;
}

void Client::onReadable() {
	//TODO
}

void Client::onWritable() {
	//TODO
}

bool Client::wantsRead() const {
	//TODO
	return false;
}

bool Client::wantsWrite() const {
	//TODO
	return false;
}

bool Client::shouldClose() const {
	//TODO
	return false;
}
