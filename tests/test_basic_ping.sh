#!/bin/env bash

make ;
valgrind --leak-check=full ./webserv 

curl 127.0.0.1:8080
echo "stop" | nc -uU /tmp/close_webserver.sock