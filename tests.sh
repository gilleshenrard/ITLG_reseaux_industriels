# This script tests all the possible outcomes between the client and the server
# A server has to be up and running :
#	clapton : 192.168.56.10, port 3490, tcp
# Made by Gilles Henrard
# Last modification : 19/11/2019

#! /bin/bash

#test 0
echo -e '\e[1m0- test of the client application with an improper number of arguments\e[0m'
echo -e '\e[1mbin/client clapton\e[0m'
bin/client clapton

#test 1
echo ''
echo -e '\e[1m1- test of the server application via hostname\e[0m'
echo -e '\e[1mbin/client clapton 3490\e[0m'
bin/client clapton 3490

#test 2
echo ''
echo -e '\e[1m2- test of the server application via direct IP\e[0m'
echo -e '\e[1mbin/client 192.168.56.10 3490\e[0m'
bin/client 192.168.56.10 3490

#test 3
echo ''
echo -e '\e[1m3- test of a non-existing hostname\e[0m'
echo -e '\e[1mbin/client dummy 3490\e[0m'
bin/client dummy 3490

#test 4
echo ''
echo -e '\e[1m4- test of a wrong IP\e[0m'
echo -e '\e[1mbin/client 456.123.1.468 3490\e[0m'
bin/client 456.123.1.468 3490

#test 5
echo ''
echo -e '\e[1m5- test with stderr ignored\e[0m'
echo -e '\e[1mbin/client 456.123.1.468 3490 2> /dev/null\e[0m'
bin/client 456.123.1.468 3490 2> /dev/null

#test 6
echo ''
echo -e '\e[1m6- test with stdout ignored\e[0m'
echo -e '\e[1mbin/client clapton 3490 1> /dev/null\e[0m'
bin/client clapton 3490 1> /dev/null

#test 7
echo ''
echo -e '\e[1m7- test of a wrong port (will timeout after 5 sec.)\e[0m'
echo -e '\e[1mbin/client clapton 79456\e[0m'
bin/client clapton 79456

#test 8
echo ''
echo -e '\e[1m8- test of a port blocked by firewall (will timeout after 5 sec.)\e[0m'
echo -e '\e[1mbin/client clapton 80\e[0m'
bin/client clapton 80
