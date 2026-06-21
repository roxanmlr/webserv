#!/usr/bin/env bash

make 
valgrind ./webserv &

sleep 5

for (( i=1; i<= 33; i++ )) do
	echo $i;
	curl localhost:8080/cgi-bin/hello.py;
	curl localhost:8080/cgi-bin/hello_p.py;
	curl localhost:8080/cgi-bin/sleep.py;
done

echo "stop" | nc -uU  /tmp/clo*sock
