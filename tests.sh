#! /bin/bash

cd bin/Debug

echo '1- test of a public daytime server via TCP port 13'
./client time-a-g.nist.gov 13 tcp

echo ''
echo '2- test of the server application via hostname, TCP, port 3490'
./client clapton 3490 tcp

echo ''
echo '3- test of the server application via direct IP, TCP, port 3490'
./client 192.168.56.10 3490 tcp

echo ''
echo '4- test of the server application via hostname, UDP, port 3490'
./client vaughan 3490 udp

echo ''
echo '5- test of the server application via direct IP, UDP, port 3490'
./client 192.168.56.11 3490 udp

echo ''
echo '6- test of a non-existing hostname'
./client dummy 3490 tcp

echo ''
echo '7- test of a wrong IP'
./client 456.123.1.468 3490 tcp

echo ''
echo '8- test of a wrong port (will timeout after 5 sec.)'
./client clapton 79456 tcp

echo ''
echo '9- test of a port blocked by firewall (will timeout after 5 sec.)'
./client clapton 80 tcp
