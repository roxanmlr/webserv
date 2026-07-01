#!/bin/env bash

#siege -r100 -c255 -f /home/lmilando/webserv/tests/urls_success.txt --internet 
siege -r100 -c2 -f ./tests/urls_success.txt --internet 
echo "stop" | nc -uU /tmp/close_webserver.sock
