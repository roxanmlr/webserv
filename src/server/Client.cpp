/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 20:23:45 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/26 06:50:16 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "../handler/UploadHandler.hpp"

Client::Client()
	: fd(-1), state(INIT), serv(NULL), lastActivity(time(NULL)), read_status(READ_OK), read_buffer(""), write_status(WRITE_NOT_START), write_pos(0),
	  write_buffer(""), cgiHandler(), cgi_status(NO_CGI), parse_state(IHttpRequest::INCOMPLETE) {

	std::map<std::string, std::string> headers;
}

Client::Client(int fd, IServerConfig const* serv)
	: fd(fd), state(INIT), serv(serv), lastActivity(time(NULL)), read_status(READ_OK), read_buffer(""), write_status(WRITE_NOT_START), write_pos(0),
	  write_buffer(""), cgiHandler(), cgi_status(NO_CGI), parse_state(IHttpRequest::INCOMPLETE) {
	std::map<std::string, std::string> headers;
}

Client::~Client() {
}

Client::Client(Client const& other)
	: fd(other.fd), state(other.state), serv(other.serv), lastActivity(other.lastActivity), read_status(other.read_status), read_buffer(other.read_buffer),
	  write_status(other.write_status), write_pos(other.write_pos), write_buffer(other.write_buffer), cgiHandler(other.cgiHandler),
	  cgi_status(other.cgi_status), parse_state(IHttpRequest::INCOMPLETE) {
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
	if (isTimeOut())
		return;
	char tmp[4096];
	while (1) {
		if ((serv && !serv->getClientMaxBodySize().empty() && read_buffer.size() >= serv->getClientMaxBodySize().get()) || read_buffer.size() >= (1 << 18)) {
			read_buffer.clear();
			read_status		   = READ_OVERFLOW;
			this->write_status = WRITE_READY;
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
			lastActivity = time(NULL);
			continue;
		}
		if (n == 0) {
			read_status = READ_CLOSED;
			std::cerr << "READ CLOSED (EOF)" << std::endl;
			break;
		}
		read_status = READ_AGAIN;
		break;
	}
	if (!(read_buffer.empty())) {
		parse_state = this->_request.feed(read_buffer.data(), read_buffer.size());
		if (parse_state == IHttpRequest::COMPLETE) {
			std::cerr << "HTTP request parsed with success" << std::endl;
			read_buffer.clear();
			read_buffer		   = this->_request.getBuffer();
			this->state		   = WRITING;
			this->write_status = WRITE_READY;
		} else if (parse_state == IHttpRequest::PARSE_ERROR) {
			std::cerr << "HTTP request error 400(bad request)" << std::endl;
			read_buffer.clear();
			this->state		   = PARSE_ERROR;
			this->write_status = WRITE_READY;
		}
	} else if (read_status == READ_ERROR) {
		this->write_status = WRITE_READY;
	}
}

void Client::onWritable() {
	if (write_status == WRITE_NOT_START)
		return;
	if (cgi_status == CGI_RUNNING)
		return;
	std::cerr << "Client on Writable pass CGI\n";
	while (write_pos == 0 && write_status == WRITE_READY && cgi_status == NO_CGI) { // TODO check if the CGI is not concerned
		Optional<ILocationConfig const*> optLoc;
		{
			if (state == TIMEOUT) {
				response.setStatus(408);
				response.setHeader("Connection", "close");
				HttpResponse::applyErrorPage(response, 408, serv);
				break;
			}
			if (state == PARSE_ERROR) {
				response.setStatus(400);
				HttpResponse::applyErrorPage(response, 400, serv);
				break;
			}
			if (read_status == READ_OVERFLOW) {
				response.setStatus(413);
				HttpResponse::applyErrorPage(response, 413, serv);
				break;
			}
			if (read_status == READ_ERROR) {
				response.setStatus(400);
				HttpResponse::applyErrorPage(response, 400, serv);
				break;
			}
			if (!serv) {
				response.setStatus(500);
				HttpResponse::applyErrorPage(response, 500, serv);
				break;
			}
			optLoc = serv->matchLocation(_request.getUri());
			if (optLoc.empty()) {
				response.setStatus(404);
				HttpResponse::applyErrorPage(response, 404, serv);
				break;
			}
		}
		if (!optLoc.empty()) {

			ILocationConfig const* bestMatch = optLoc.get();
			{
				StaticFileHandler  staticFileHandler;
				CgiHandler		   cgiHandler;
				UploadHandler	   uploadHandler;
				RedirectionHandler redirectionHandler;
				DeleteHandler	   deleteHandler;

				if (redirectionHandler.canHandle(_request, *bestMatch)) {
					if (!bestMatch->isMethodAllowed(_request.getMethod())) {
						response.setStatus(405);
						HttpResponse::applyErrorPage(response, 405, serv);
						break;
					}
					redirectionHandler.handle(_request, *bestMatch, response, serv);
					break;
				}
				if (cgiHandler.canHandle(_request, *bestMatch)) { // TODO check it up
					if (cgi_status == NO_CGI)
						return;
				}
				if (deleteHandler.canHandle(_request, *bestMatch)) {
					if (!bestMatch->isMethodAllowed(_request.getMethod())) {
						response.setStatus(405);
						HttpResponse::applyErrorPage(response, 405, serv);
						break;
					}
					deleteHandler.handle(_request, *bestMatch, response, serv);
					break;
				}
				if (staticFileHandler.canHandle(_request, *bestMatch)) {
					std::cerr << "Static file handler\n";
					if (!bestMatch->isMethodAllowed(_request.getMethod())) {
						response.setStatus(405);
						HttpResponse::applyErrorPage(response, 405, serv);
						break;
					}
					staticFileHandler.handle(_request, *bestMatch, response, serv);
					break;
				}
				if (uploadHandler.canHandle(_request, *bestMatch)) {
					if (!bestMatch->isMethodAllowed(_request.getMethod())) {
						response.setStatus(405);
						HttpResponse::applyErrorPage(response, 405, serv);
						break;
					}
					uploadHandler.handle(_request, *bestMatch, response, serv);
					break;
				}
				response.setStatus(405);
				HttpResponse::applyErrorPage(response, 405, serv);
				break;
			}
		}
	}
	if (write_status == WRITE_READY) // TODO check if the CGI is on and is finished
		write_buffer = response.serialize();
	while (write_pos < write_buffer.size()) {
		const char* data = write_buffer.data() + write_pos;
		size_t		size = write_buffer.size() - write_pos;
		std::cerr << "Sending\n\t" << data << "\n";
		ssize_t n = send(fd, data, size, MSG_NOSIGNAL);
		if (n > 0) {
			write_pos += n;
			write_status = WRITE_OK;
			continue;
		}
		if (n == 0) {
			write_status = WRITE_AGAIN;
			break;
		}
		write_status = WRITE_AGAIN;
		break;
	}
	if (write_pos == write_buffer.size()) {
		write_buffer.clear();
		write_pos	 = 0;
		write_status = (write_status != WRITE_ERROR) ? WRITE_DONE : WRITE_ERROR;
	}
}

bool Client::wantsRead() const {
	if (state == WRITING || state == PARSE_ERROR)
		return false;
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
	case WRITE_READY:
		return true;
	case WRITE_NOT_START:
		return false;
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
	return !wantsRead() && !wantsWrite() && cgi_status != CGI_RUNNING;
}

void Client::setWriteBuffer(std::string const& write_buffer) {
	this->write_buffer = write_buffer;
}

bool Client::isTimeOut() {
	if (cgi_status != CGI_RUNNING)
		return false;
	if (serv && !serv->getTimeOut().empty()) {
		double diff = std::difftime(time(NULL), lastActivity);
		if (diff < 0)
			diff = -diff;
		if (serv->getTimeOut().get() <= static_cast<size_t>(diff)) {
			state			   = TIMEOUT;
			read_status		   = READ_ERROR;
			this->write_status = WRITE_READY;
			return true;
		}
	}
	return false;
}

bool Client::shouldBeHandleByCGI() {
	if (cgi_status != NO_CGI) {
		return false;
	}
	std::cerr << "Test si la requete peut etre gere par CGI\n";
	if (parse_state != IHttpRequest::COMPLETE) {
		std::cerr << "Parsing incomplet\n";
		return false;
	}
	Optional<ILocationConfig const*> optLoc = serv->matchLocation(_request.getUri());
	if (optLoc.empty())
		return false;
	ILocationConfig const* bestMatch = optLoc.get();
	if (cgiHandler.canHandle(_request, *bestMatch)) {
		std::cerr << "La requête doit être geree par CGI\n";
		return true;
	}
	std::cerr << "La requête ne doit pas être geree par CGI\n";
	return false;
}

void Client::handleByCGI() {
	if (cgi_status != NO_CGI) {
		std::cerr << "Requete deja gere\n";
		exit(1);
	}
	std::cerr << "Lancement du CGI\n";
	cgi_status								= CGI_RUNNING;
	Optional<ILocationConfig const*> optLoc = serv->matchLocation(_request.getUri());
	if (optLoc.empty())
		return;
	ILocationConfig const* bestMatch = optLoc.get();
	if (!bestMatch->isMethodAllowed(_request.getMethod())) {
		std::cerr << "Method not allowed fin du CGI\n";
		HttpResponse::applyErrorPage(response, 405, serv);
		cgi_status = CGI_FINISHED;
	}
	if (!cgiHandler.handle(_request, *bestMatch, response, serv)) {
		std::cerr << "Echec du lancement du CGI\n";
		cgi_status = CGI_FINISHED;
		cgiHandler.fillResponse(response);
		std::cerr << "response echec" << response.serialize() << "\n";
	}
	std::cerr << "Fin lancement CGI\n";
}

void Client::getCgiFd(int& input, int& output) {
	input  = cgiHandler.getInputFd();
	output = cgiHandler.getOutputFd();
}

bool Client::onCgiInput() {
	return cgiHandler.onInput();
}

bool Client::onCgiOutput() {
	return cgiHandler.onOutput();
}

bool Client::isCgiFinished() {
	if (cgiHandler.isFinished()) {
		std::cerr << "CGI Ended\n";
		cgiHandler.fillResponse(response);
		std::cerr << response.serialize() << "\n";
		cgi_status = CGI_FINISHED;
		return true;
	}
	return false;
}
