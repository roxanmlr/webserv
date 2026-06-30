#!/bin/env bash

wrk -t4 -c1000 -d30s $1
echo "stop" | nc -uU -q 0 /tmp/close_webserver.sock

