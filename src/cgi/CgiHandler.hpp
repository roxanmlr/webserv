#pragma once
#include "../config/Config.hpp"
#include "../server/WebServerError.hpp"
#include "../utils/utils.hpp"
#include "ICgiHandler.hpp"
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#define TIME_OUT_CGI 5

class CgiHandler : public ICgiHandler {

private:
	ILocationConfig::CgiPass cgipass;
	int						 pid;
	enum State { INIT, PROCESSING, PROCESS_END, FINISHED, TIMEOUT, ERROR };
	State				 state;
	std::string			 output;
	int					 pipefd[2];
	int					 outfile[2];
	bool				 write_finished;
	char*				 bufwrite;
	size_t				 bufwrite_pos;
	size_t				 bufwrite_size;
	bool				 read_finished;
	IServerConfig const* serv;
	time_t				 s_time;
	void				 hasTimeOut();

public:
	~CgiHandler();
	CgiHandler();
	CgiHandler(const CgiHandler& base);
	CgiHandler& operator=(const CgiHandler& base);

	bool		canHandle(const IHttpRequest& req, const ILocationConfig& loc);
	bool		handle(const IHttpRequest& req, const ILocationConfig& loc, IHttpResponse& res, IServerConfig const* serv);
	int			getInputFd() const;	 // pipe vers STDIN du CGI
	int			getOutputFd() const; // pipe vers STDOUT du CGI
	bool		isFinished();
	void		onInput();	// appelé sur POLLOUT du pipe stdin
	void		onOutput(); // appelé sur POLLOUT du pipe stdout
	void		fillResponse(IHttpResponse& res);
};