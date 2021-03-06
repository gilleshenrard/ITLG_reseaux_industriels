# This script tests all the possible outcomes between the client and the server
# Made by Gilles Henrard
# Last modification : 11/01/2020

# Set up
# The assignment is programmed in C98, compiled with GNU GCC.
# The code has been tested on :
# * client : Ubuntu 18.04 LTS Desktop version, directly on the host
# * Server : Ubuntu 18.04 LTS Server version, in a Virtualbox VM
#
# A host-only connection (192.168.56.0/24) has been created to allow machines to communicate.
# The IP addresses are set as followed :
# * host : 192.168.56.1/24
# * clapton server (TCP) : 192.168.56.10/24
# Note that the server hostname has to be added in /etc/hosts

# The code is edited and compiled in a directory shared with the guest, so it is centralised and only one version is edited and executed.
# Do not forget to execute "export LD_LIBRARY_PATH=../lib" to use a debugger with Code::Blocks

#! /bin/bash

#
# Errors tests
#

#test 1
echo -e '\e[1m1- test of the client application with an improper number of arguments\e[0m'
echo -e '\e[1mbin/client clapton\e[0m'
bin/client clapton

#test 2
echo ''
echo -e '\e[1m2- test of a non-existing hostname\e[0m'
echo -e '\e[1mbin/client dummy 3490\e[0m'
bin/client dummy 3490

#test 3
echo ''
echo -e '\e[1m3- test of a wrong IP\e[0m'
echo -e '\e[1mbin/client 456.123.1.468 3490\e[0m'
bin/client 456.123.1.468 3490

#test 4
echo ''
echo -e '\e[1m4- test with stderr ignored\e[0m'
echo -e '\e[1mbin/client 456.123.1.468 3490 2> /dev/null\e[0m'
bin/client 456.123.1.468 3490 2> /dev/null

#test 5
echo ''
echo -e '\e[1m5- test of a wrong port (will timeout after 5 sec.)\e[0m'
echo -e '\e[1mbin/client clapton 79456\e[0m'
bin/client clapton 79456

#test 6
echo ''
echo -e '\e[1m6- test of a port blocked by firewall (will timeout after 5 sec.)\e[0m'
echo -e '\e[1mbin/client clapton 80\e[0m'
bin/client clapton 80

#
# Successes tests
#

#test 7
echo ''
echo -e '\e[1m7- test of the server application via hostname\e[0m'
echo -e '\e[1mbin/client clapton 3490\e[0m'

echo "Before the transfer, data/ contains:"
ls -l data/
echo ""

bin/client clapton 3490

echo "After the transfer, data/ contains:"
ls -l data/

diff -u music.mp3 data/music.mp3
if [[ $? -eq 0 ]]
then
	echo "Source and destination files are equal"
fi

