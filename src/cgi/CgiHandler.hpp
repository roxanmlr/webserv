#pragma once
#include "../config/Config.hpp"
#include "../server/WebServerError.hpp"
#include "../utils/utils.hpp"
#include "ICgiHandler.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

class CgiHandler : public ICgiHandler {

private:
	ILocationConfig::CgiPass cgipass;
	int						 input_fd;
	int						 output_fd;
	bool					 _isfinished;

public:
	~CgiHandler();
	CgiHandler();
	CgiHandler(const CgiHandler& base);
	CgiHandler& operator=(const CgiHandler& base);

	bool		canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	bool		handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
	int			getInputFd() const;	 // pipe vers STDIN du CGI
	int			getOutputFd() const; // pipe vers STDOUT du CGI
	bool		isFinished() const;
	void		onInputWritable();	// appelé sur POLLOUT du pipe stdin
	void		onOutputWritable(); // appelé sur POLLOUT du pipe stdout
};