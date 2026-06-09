#!/bin/env bash

siege -r100 -c255 $1
echo "stop" | nc -uU -q 0 /tmp/close_webserver.sock
