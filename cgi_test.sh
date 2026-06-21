#!/usr/bin/env bash

make 
valgrind ./webserv &

sleep 5

for (( i=1; i<= 100; i++ )) do
	echo $i;
	curl localhost:8080/cgi-bin/hello.py;
done

echo "stop" | nc -uU -q 0 /tmp/clo*sock
