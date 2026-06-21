#!/usr/bin/env python3

import os
import sys
import urllib.parse

def read_body():
    try:
        content_length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        content_length = 0

    if content_length > 0:
        return sys.stdin.read(content_length)

    return ""

method = os.environ.get("REQUEST_METHOD", "")
query_string = os.environ.get("QUERY_STRING", "")
content_type = os.environ.get("CONTENT_TYPE", "")

body = read_body()

query_params = urllib.parse.parse_qs(query_string)

print("Content-Type: text/html")
print()

print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")

print("<h1>CGI Python OK</h1>")

print("<h2>Request info</h2>")
print("<ul>")
print(f"<li>Method: {method}</li>")
print(f"<li>Query string: {query_string}</li>")
print(f"<li>Content-Type: {content_type}</li>")
print(f"<li>Content-Length: {os.environ.get('CONTENT_LENGTH', '')}</li>")
print("</ul>")

print("<h2>Query parameters</h2>")
print("<pre>")
print(query_params)
print("</pre>")

print("<h2>Body</h2>")
print("<pre>")
print(body)
print("</pre>")

print("<h2>Environment</h2>")
print("<pre>")
for key in sorted(os.environ.keys()):
    print(f"{key}={os.environ[key]}")
print("</pre>")

print("</body>")
print("</html>")
