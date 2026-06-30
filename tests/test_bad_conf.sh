 #!/bin/env bash

make ;
DIR="${1:-config/bad}"
for file in "$DIR"/* ; do
	echo ========== $file ==========;
	echo ;
	./webserver $file;
	echo ==== Leaks check ====
	valgrind  --leak-check=full ./webserv $file 2>&1 | grep 'at exit'
	echo;
done
