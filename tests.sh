#! /bin/bash

cd bin/Debug

echo '0- test of the client application with an improper number of arguments'
./client clapton

echo ''
echo '1- test of the server application via hostname, TCP, port 13'
./client clapton 13 tcp "this is a test"

echo ''
echo '2- test of the server application via direct IP, TCP, port 13'
./client 192.168.56.10 13 tcp "this is a test"

echo ''
echo '3- test of the server application via hostname, UDP, port 13'
./client vaughan 13 udp "this is a test"

echo ''
echo '4- test of the server application via direct IP, UDP, port 13'
./client 192.168.56.11 13 udp "this is a test"

echo ''
echo '5- test of a non-existing hostname'
./client dummy 13 tcp "this is a test"

echo ''
echo '6- test of a wrong IP'
./client 456.123.1.468 13 tcp "this is a test"

echo ''
echo '7- test of a wrong port (will timeout after 5 sec.)'
./client clapton 79456 tcp "this is a test"

echo ''
echo '8- test of a port blocked by firewall (will timeout after 5 sec.)'
./client clapton 80 tcp "this is a test"
