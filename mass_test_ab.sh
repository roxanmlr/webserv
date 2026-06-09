#!/bin/env bash

ab -n 10000 -c 100 $1
echo "stop" | nc -uU -q 0 /tmp/close_webserver.sock

