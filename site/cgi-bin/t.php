#!/usr/bin/php-cgi
<?php

header('Content-Type: text/plain');

echo "CGI test OK\n";
echo "Method: " . ($_SERVER['REQUEST_METHOD'] ?? '') . "\n";
echo "Query: " . ($_SERVER['QUERY_STRING'] ?? '') . "\n";
echo "Body: " . file_get_contents('php://input') . "\n";
