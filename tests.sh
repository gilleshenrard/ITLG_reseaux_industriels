# This script tests all the possible outcomes between the client and the server
# Two servers have to be up and running :
#	clapton : 192.168.56.10, port 7, tcp
#	vaughan : 192.168.56.11, port 7, udp
# Made by Gilles Henrard
# Last modification : 01/11/2019

#! /bin/bash

cd bin/Debug

#test 0
echo -e '\e[1m0- test of the client application with an improper number of arguments\e[0m'
echo -e '\e[1m./client clapton\e[0m'
./client clapton

#test 1
echo ''
echo -e '\e[1m1- test of the server application via hostname, TCP, port 7\e[0m'
echo -e '\e[1m./client clapton 7 tcp "this is the test 1"\e[0m'
./client clapton 7 tcp "this is the test 1"

#test 2
echo ''
echo -e '\e[1m2- test of the server application via direct IP, TCP, port 7\e[0m'
echo -e '\e[1m./client 192.168.56.10 7 tcp "this is the test 2"\e[0m'
./client 192.168.56.10 7 tcp "this is the test 2"

#test 3
echo ''
echo -e '\e[1m3- test of the server application via hostname, TCP, protocol name\e[0m'
echo -e '\e[1m./client clapton "echo" tcp "this is the test 3"\e[0m'
./client clapton "echo" tcp "this is the test 3"

#test 4
echo ''
echo -e '\e[1m4- test of the server application via hostname, UDP, port 7\e[0m'
echo -e '\e[1m./client vaughan 7 udp "this is the test 4"\e[0m'
./client vaughan 7 udp "this is the test 4"

#test 5
echo ''
echo -e '\e[1m5- test of the server application via direct IP, UDP, port 7\e[0m'
echo -e '\e[1m./client 192.168.56.11 7 udp "this is the test 5"\e[0m'
./client 192.168.56.11 7 udp "this is the test 5"

#test 6
echo ''
echo -e '\e[1m6- test of a non-existing hostname\e[0m'
echo -e '\e[1m./client dummy 7 tcp "this is the test 6"\e[0m'
./client dummy 7 tcp "this is the test 6"

#test 7
echo ''
echo -e '\e[1m7- test of a wrong IP\e[0m'
echo -e '\e[1m./client 456.123.1.468 7 tcp "this is the test 7"\e[0m'
./client 456.123.1.468 7 tcp "this is the test 7"

#test 8
echo ''
echo -e '\e[1m7- test with stderr ignored\e[0m'
echo -e '\e[1m./client 456.123.1.468 7 tcp "this is the test 7" 2> /dev/null\e[0m'
./client 456.123.1.468 7 tcp "this is the test 7" 2> /dev/null

#test 9
echo ''
echo -e '\e[1m1- test with stdout ignored\e[0m'
echo -e '\e[1m./client clapton 7 tcp "this is the test 9" 1> /dev/null\e[0m'
./client clapton 7 tcp "this is the test 9" 1> /dev/null

#test 10
echo ''
echo -e '\e[1m8- test of a wrong port (will timeout after 5 sec.)\e[0m'
echo -e '\e[1m./client clapton 79456 tcp "this is the test 8"\e[0m'
./client clapton 79456 tcp "this is the test 8"

#test 11
echo ''
echo -e '\e[1m9- test of a port blocked by firewall (will timeout after 5 sec.)\e[0m'
echo -e '\e[1m./client clapton 80 tcp "this is the test 9"\e[0m'
./client clapton 80 tcp "this is the test 9"
