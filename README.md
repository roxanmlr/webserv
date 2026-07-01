_This project has been created as part of the 42 curriculum by lmilando and mzouhir._
# Description

This project, written in C++98, allowed us to introduce ourselves to the Web and the HTTP protocol, and to practice C++ within a complex project.
It notably helped us improve our architectural skills by defining the software product based on a specification, specifically the RFC.

## Table of Contents
- [Overview](#overview)
- [File Organization](#file-organization)
- [Modules and Interfaces](#modules-and-interfaces)
- [Request Flow](#request-flow)
- [Critical Points of the Subject](#critical-points-of-the-subject)

---

## Overview

The architecture relies on **modules = an abstract interface + one (or more) concrete implementation(s)**. This allows:
- Decoupling modules;
- Testing each module independently (easy mocks);
- Adding new handlers (by method, by resource type) without touching the rest.

The single orchestrator (`WebServer`) holds the one and only `epoll()` loop. All network I/O (listening sockets, client sockets, CGI pipes) goes through this epoll.

```

main.cpp
└── IConfigParser → IConfig
└── IWebServer (orchestrator, holds the epoll loop)
├── IServerSocket[] (one per listening port)
├── IClient[]       (active connections)
│   ├── IHttpRequest (incremental parser)
│   └── IHttpResponse (builder)
├── IRequestHandler (dispatch GET / POST / DELETE)
└── ICgiHandler       (classic CGI via fork+execve)

```

---
## File Organization

Each module follows the convention: `IModule.hpp` (interface) + `Module.hpp/.cpp` (implementation)

```

.
├── config
│   ├── bad
│   │   ├── allow_methods_duplicate_method.conf
│   │   ├── allow_methods_no_method.conf
│   │   ├── allow_methods_unknown_method.conf
│   │   ├── autoindex_invalid_value.conf
│   │   ├── cgi_pass_duplicate_extension.conf
│   │   ├── cgi_pass_missing_interpreter.conf
│   │   ├── client_max_body_size_duplicate.conf
│   │   ├── client_max_body_size_invalid_suffix.conf
│   │   ├── client_max_body_size_invalid_value.conf
│   │   ├── error_page_missing_code.conf
│   │   ├── fastcgi_param_duplicate_key.conf
│   │   ├── fastcgi_pass_duplicate.conf
│   │   ├── index_duplicate.conf
│   │   ├── listen_invalid_port.conf
│   │   ├── listen_port_zero.conf
│   │   ├── location_duplicate.conf
│   │   ├── location_missing_opening_brace.conf
│   │   ├── location_regex_modifier_unsupported.conf
│   │   ├── missing_semicolon.conf
│   │   ├── missing_server_closing_brace.conf
│   │   ├── missing_server_opening_brace.conf
│   │   ├── no_server_block.conf
│   │   ├── return_duplicate.conf
│   │   ├── return_non_integer_code.conf
│   │   ├── root_duplicate.conf
│   │   ├── server_listening_same_port.conf
│   │   ├── server_name_duplicate.conf
│   │   ├── unexpected_token_after_server.conf
│   │   ├── unknown_location_directive.conf
│   │   ├── unknown_server_directive.conf
│   │   └── upload_store_duplicate.conf
│   ├── default.conf
│   ├── docker.conf
│   └── simple.conf
├── Makefile
├── README.md
├── site
│   └── cgi-bin
│       ├── hello_p.py
│       ├── hello.py
│       ├── sleep.py
│       ├── test.php
│       └── t.php
├── src
│   ├── cgi
│   │   ├── CgiHandler.cpp
│   │   ├── CgiHandler.hpp
│   │   └── ICgiHandler.hpp
│   ├── config
│   │   ├── Config.cpp
│   │   ├── ConfigError.cpp
│   │   ├── ConfigError.hpp
│   │   ├── Config.hpp
│   │   ├── ConfigParser.cpp
│   │   ├── ConfigParser.hpp
│   │   ├── IConfig.cpp
│   │   ├── IConfig.hpp
│   │   ├── IConfigParser.hpp
│   │   ├── ILocationConfig.hpp
│   │   ├── IServerConfig.hpp
│   │   ├── LocationConfigBuilder.cpp
│   │   ├── LocationConfigBuilder.hpp
│   │   ├── LocationConfig.cpp
│   │   ├── LocationConfig.hpp
│   │   ├── ServerConfigBuilder.cpp
│   │   ├── ServerConfigBuilder.hpp
│   │   ├── ServerConfig.cpp
│   │   ├── ServerConfig.hpp
│   │   ├── Tokenizer.cpp
│   │   └── Tokenizer.hpp
│   ├── fastcgi
│   │   ├── IFastCgiClient.hpp
│   │   └── IFastCgiProtocol.hpp
│   ├── handler
│   │   ├── DeleteHandler.cpp
│   │   ├── DeleteHandler.hpp
│   │   ├── IRequestHandler.hpp
│   │   ├── RedirectionHandler.cpp
│   │   ├── RedirectionHandler.hpp
│   │   ├── StaticFileHandler.cpp
│   │   ├── StaticFileHandler.hpp
│   │   ├── UploadHandler.cpp
│   │   └── UploadHandler.hpp
│   ├── http
│   │   ├── HttpRequest.cpp
│   │   ├── HttpRequest.hpp
│   │   ├── HttpResponse.cpp
│   │   ├── HttpResponse.hpp
│   │   ├── IHttpRequest.hpp
│   │   └── IHttpResponse.hpp
│   ├── main.cpp
│   ├── server
│   │   ├── Client.cpp
│   │   ├── Client.hpp
│   │   ├── IClient.hpp
│   │   ├── IWebServer.hpp
│   │   ├── WebServer.cpp
│   │   ├── WebServerError.cpp
│   │   ├── WebServerError.hpp
│   │   └── WebServer.hpp
│   └── utils
│       ├── ft_itoa.cpp
│       ├── Logger.cpp
│       ├── Logger.hpp
│       ├── Optional.tpp
│       ├── set_nonblocking.cpp
│       └── utils.hpp
├── tests
│   ├── cgi_test.sh
│   ├── mass_test_ab.sh
│   ├── mass_test_siege.sh
│   ├── mass_test_wrk.sh
│   ├── test_bad_conf.sh
│   └── test_basic_ping.sh
└── webserv

```
## Modules and Interfaces
### Config Module
#### `IConfigParser`
Loads a configuration file and produces an `IConfig`. Exposes a single method `parse(path)` that returns a pointer to `IConfig`. Used only by `main.cpp` at startup.

#### `IConfig`
Represents the loaded configuration. Gives access to `IServerConfig` (one per `server` block), with their `LocationConfig` (one per `location` block). Allows `IWebServer` to resolve the correct server block from the `Host` header via `findServer(host, port)`.

`IServerConfig` contains: ports, `server_name`, error pages, `client_max_body_size`, list of `LocationConfig`.
`LocationConfig` contains: `root`, `index`, allowed methods, `autoindex`, redirections, CGI parameters, upload path.

---

### Server Module
#### `IWebServer`
Receives an `IConfig` at initialization. Creates the `ListenConfig` for each unique `host:port` pair, then enters the infinite `epoll_wait()` loop via `run()`. Dispatches events to `IClient` and `ICgiHandler` depending on the fd type.

#### `WebServer::ListenConfig`
Internal structure of `WebServer` that wraps a socket in a `LISTEN` state, its associated `host:port`, as well as the associated `IServerConfig` so that `IWebServer` knows which server blocks apply to this port.

Setup: `socket()` → `setsockopt(SO_REUSEADDR)` → `fcntl(O_NONBLOCK)` → `bind()` → `listen()`.

#### `IClient`
Represents an active HTTP connection. `IWebServer` calls it on `onReadable()` (POLLIN) and `onWritable()` (POLLOUT). The internal state (`READING_HEADERS`, `READING_BODY`, `PROCESSING`, `WRITING`, `CLOSING`) determines what `wantsRead()`, `wantsWrite()`, and `shouldClose()` return, allowing `IWebServer` to update the `pollfd` mask without knowing the details of the HTTP protocol.

### HTTP Module
#### `IHttpRequest`
**Incremental** parser: data arrives in chunks via epoll. `feed(data, len)` accumulates bytes and returns `INCOMPLETE`, `COMPLETE`, or `PARSE_ERROR`. Once `COMPLETE`, `IClient` can query `getMethod()`, `getUri()`, `getHeader()`, and `getBody()` to build the response via `IRequestHandler`.

Internal states: request line → headers → body (handles `Content-Length` **and** `Transfer-Encoding: chunked`).

#### `IHttpResponse`
HTTP response builder. `IRequestHandler` calls `setStatus()`, `setHeader()`, and `setBody()`, then `IClient` serializes everything via `serialize()` to send it to the socket. Handles mandatory headers: `Date`, `Server`, `Content-Length` or `Transfer-Encoding`, `Connection`.

### Handler Module
#### `IRequestHandler`
Common interface for all handlers. `IWebServer` (via the `RequestHandler` dispatcher) first calls `canHandle(req, loc)` on each registered handler, then delegates to `handle(req, loc, res)` on the first one that accepts. The response is built directly into the provided `IHttpResponse`.

Concrete implementations:
- **`StaticFileHandler`**: GET on files, autoindex, MIME types.
- **`UploadHandler`**: POST with `multipart/form-data` or raw body.
- **`DeleteHandler`**: DELETE on a resource.
- **`RedirectionHandler`**: Configured `return 301/302`.
- **`CgiHandler`**: Classic CGI execution (fork + execve).

The `Client` keeps the list of handlers and delegates to the first one whose `canHandle()` returns `true`.

### CGI Module

#### `ICgiHandler`
Extends `IRequestHandler`. In addition to `handle()`, it exposes `getInputFd()` and `getOutputFd()` (the two pipes) so that `IWebServer` can add them to the `pollfd` array. `onInputWritable()` is called on `POLLOUT` of the stdin pipe (sending the POST body), `onOutputReadable()` on `POLLIN` of the stdout pipe (reading the CGI response). When `isFinished()` becomes `true`, `IWebServer` removes the pipes from the epoll and wakes up the HTTP `IClient`.

Workflow:
1. Creates two `pipe()` calls, marks them `O_NONBLOCK`.
2. `fork()` then in the child: `dup2()` + `execve()` of the binary (`php-cgi`, `python`, …).
3. Pipes are added to the main `epoll()` — mapped to the waiting `IClient`.
4. The CGI process itself is not in epoll (`non-blocking waitpid` or `SIGCHLD`).

---

### Utils Module

#### `ILogger`
Allows easily plugging in a file logger, stderr, or a silent logger for tests. Exposes a single method `log(level, msg)` with levels `DEBUG`, `INFO`, `WARN`, `ERROR`. Injected by dependency into all modules that need it, allowing tests to use a silent `MockLogger`.

---

## Request Flow

### Static Request
1. `epoll()` returns, an `IServerSocket` has `POLLIN` → `acceptClient()` → create an `IClient`, add it to the epoll in `POLLIN`.
2. `POLLIN` on the client fd → `client.onReadable()` → internally: `recv()` + `request.feed(buffer)`.
3. When `feed()` returns `COMPLETE` → resolve the `IServerConfig` (via `Host` header) → dispatch to the matching `IRequestHandler` → response is serialized into the write buffer → `wantsWrite()` becomes `true` → `WebServer` puts the fd in `POLLOUT`.
4. `POLLOUT` → `client.onWritable()` → `send()` what we can. When everything is sent: if keep-alive, reset the state and go back to `POLLIN`; otherwise `shouldClose()` becomes `true` and `WebServer` closes.

---

## Critical Points of the Subject

### Single `epoll()`
Not one epoll per thread, not one epoll for clients and another for CGIs. Everything is in the same `pollfd` array. Hence the importance of the `fd → ConnectionType*` map which distinguishes: listen socket, HTTP client, CGI pipe.

### Non-blocking Everywhere
All sockets (listen, accept, CGI pipes) are marked `O_NONBLOCK` **right from their creation**.

### Body Parsing
Do not read `Content-Length` bytes all at once. Read what `recv` provides, accumulate, and consider the request complete only at the correct size. Same applies for chunked encoding.

### CGI and `POLLOUT`
If the backend is waiting for stdin (POST body), the pipe/socket **to** the backend must also be in epoll in `POLLOUT`. Otherwise, a deadlock is possible if the body is large.

### Timeouts
No request should hang indefinitely. `last_activity` tracking per client, and closing beyond a certain threshold.


# Instructions

> make

Modify the configuration file to match your website's root path.

> ./webserv [configuration file]

If no file is provided, the server loads `config/default.conf`.

To stop the server, you should do :
> echo "stop" | nc -uU  /tmp/close_webserver.sock

Tests are located in the directory test:
> ./tests/mass_test_siege.sh
> ./tests/curl_upload.sh
> ./tests/test_bad_conf.sh
And so on ...

You can also use the following commands:
> make clean # clean the project
> make fclean # clean and delete the executable
> make re # cleanly rebuild the project


# Resources

We used the following resources:
1. https://nginx.org/en/docs/
2. https://developer.mozilla.org/fr/docs/Web/HTTP/Reference/Status
3. https://datatracker.ietf.org/doc/html/rfc1945
4. http://manpagesfr.free.fr/man/man7/epoll.7.html
5. https://www.geeksforgeeks.org/computer-networks/simple-client-server-application-in-c/

## AI Usage
During this project, AI was used strictly as a support tool for learning and translating:

**Documentation**: AI was used to help structure, format, and translate this README file.

**HTML**: AI as been used to help building html test.

**Important Note**: Absolutely no code was generated by AI. All the code and algorithmic implementations in this project were entirely written by us.

```