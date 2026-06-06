/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:45 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/06 11:52:57 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: fd(-1), state(INIT), serv(NULL), lastActivity(time(NULL)), read_status(READ_OK), read_buffer(""), write_status(WRITE_OK), write_pos(0), write_buffer("") {
}

Client::Client(int fd, IServerConfig const* serv)
	: fd(fd), state(INIT), serv(serv), lastActivity(time(NULL)), read_status(READ_OK), read_buffer(""), write_status(WRITE_OK), write_pos(0), write_buffer("") {
}

Client::~Client() {
}

Client::Client(Client const& other)
	: fd(other.fd), state(other.state), serv(other.serv), lastActivity(other.lastActivity), read_status(other.read_status), read_buffer(other.read_buffer),
	  write_status(other.write_status), write_pos(other.write_pos), write_buffer(other.write_buffer) {
}

Client& Client::operator=(Client const& other) {
	if (this == &other)
		return *this;
	this->fd		   = other.fd;
	this->state		   = other.state;
	this->serv		   = other.serv;
	this->lastActivity = other.lastActivity;
	this->read_status  = other.read_status;
	this->read_buffer  = other.read_buffer;
	this->write_status = other.write_status;
	this->write_pos	   = other.write_pos;
	this->write_buffer = other.write_buffer;
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
	lastActivity = time(NULL);
	char tmp[4096];
	while (read_status != READ_ERROR) {
		if ((serv && !serv->getClientMaxBodySize().empty() && read_buffer.size() >= serv->getClientMaxBodySize().get()) || read_buffer.size() >= (1 << 18)) {
			read_buffer.clear();
			read_status = READ_OVERFLOW;
			std::cerr << "READ OVERFLOW" << std::endl;
			break;
		}
		ssize_t n = recv(fd, tmp, sizeof(tmp), 0);
		std::cerr << "n reads = " << n << std::endl;
		if (n > 0) {
			read_status = READ_OK;
			read_buffer.append(tmp, n);
			std::string t;
			t.append(tmp, n);
			std::cerr << "READ OK : " << t << std::endl;
			continue;
		}
		if (n == 0) {
			read_status = READ_CLOSED;
			std::cerr << "READ CLOSED" << std::endl;
			break;
		}
		if (errno == EINTR)
			continue;
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			read_status = READ_AGAIN;
			std::cerr << "READ AGAIN" << std::endl;
			break;
		}
		read_status = READ_ERROR;
		std::cerr << "READ ERROR" << std::endl;
		break;
	}
}

void Client::onWritable() {
	lastActivity = time(NULL);
	std::cerr << "on Writable" << std::endl;
	while (write_pos < write_buffer.size()) {
		const char* data = write_buffer.data() + write_pos;
		size_t		size = write_buffer.size() - write_pos;
		ssize_t		n	 = send(fd, data, size, MSG_NOSIGNAL);
		if (n > 0) {
			write_pos += n;
			write_status = WRITE_OK;
			continue;
		}
		if (n == 0) {
			write_status = WRITE_AGAIN;
			break;
		}
		if (errno == EINTR)
			continue;
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			write_status = WRITE_AGAIN;
			break;
		}
		write_status = WRITE_ERROR;
		write_pos	 = 0;
		write_buffer.clear();
		break;
	}
	if (write_status != WRITE_ERROR && write_pos == write_buffer.size()) {
		write_buffer.clear();
		write_pos	 = 0;
		write_status = WRITE_DONE;
	}
}

bool Client::wantsRead() const {
	switch (read_status) {
	case READ_OK:
	case READ_AGAIN:
		return true;
	case READ_ERROR:
	case READ_CLOSED:
	case READ_OVERFLOW:
	default:
		return false;
	}
}

bool Client::wantsWrite() const {
	switch (write_status) {
	case WRITE_OK:
	case WRITE_AGAIN:
		return write_pos < write_buffer.size();
	case WRITE_DONE:
	case WRITE_ERROR:
	default:
		return false;
	}
}

bool Client::shouldClose() const {
	return !wantsRead() && !wantsWrite();
}

void Client::setWriteBuffer(std::string const& write_buffer) {
	this->write_buffer = write_buffer;
}