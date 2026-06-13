#pragma once
#include "../config/Config.hpp"
#include "../server/WebServerError.hpp"
#include "IRequestHandler.hpp"
#include <sys/wait.h>
#include <unistd.h>

class CgiHandler : public IRequestHandler {

private:
	ILocationConfig::CgiPass cgipass;

public:
	~CgiHandler();
	CgiHandler();
	CgiHandler(const CgiHandler& base);
	CgiHandler& operator=(const CgiHandler& base);

	bool		canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	bool		handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
};