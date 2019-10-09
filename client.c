/*
** client.c
** Connects to a server via a stream socket and awaits for a 'hello world' reply
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include "network.h"

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	//checks if the port number has been provided
	if (argc != 2)
	{
		fprintf(stderr,"usage: client hostname\n");
		exit(EXIT_FAILURE);
	}

    //prepare the structure holding socket information
	// any potocol, stream socket, hostname set as argument, rest to 0
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	//format socket information and store it in list servinfo
	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

    // loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
        //generate a socket file descriptor
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				     p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		//connect to the server via the socket created
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	//no socket available
	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//translate IPv4 and IPv6 on the fly depending on the client request
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		  s, sizeof s);
	printf("client: connecting to %s\n", s);

	// all done with this structure
	freeaddrinfo(servinfo);

	//receive message from the server
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
		perror("recv");
		exit(EXIT_FAILURE);
	}

	buf[numbytes] = '\0';
	printf("client: received '%s'\n",buf);
	close(sockfd);
	return 0;
}
