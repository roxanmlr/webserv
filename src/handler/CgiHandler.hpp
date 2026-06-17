#pragma once
#include "../config/Config.hpp"
#include "../server/WebServerError.hpp"
#include "../utils/utils.hpp"
#include "IRequestHandler.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

class CgiHandler : public IRequestHandler {

private:
	static const int         TIMEOUT_SEC;
	ILocationConfig::CgiPass cgipass;

public:
	~CgiHandler();
	CgiHandler();
	CgiHandler(const CgiHandler& base);
	CgiHandler& operator=(const CgiHandler& base);

	bool		canHandle(const IHttpRequest& req, const ILocationConfig& loc, IServerConfig const* serv);
	bool		handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
};