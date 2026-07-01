#!/bin/bash

rm -f fichier.bin
dd if=/dev/zero of=fichier.bin bs=1024 count=1

curl -X POST http://localhost:8080/upload/hello \
     --header "Content-Type: application/octet-stream" \
     --data-binary "@fichier.bin"
echo "stop" | nc -uU /tmp/close_webserver.sock
