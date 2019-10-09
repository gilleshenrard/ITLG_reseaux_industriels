/*
** client.c
** Connects to a server via a stream socket and awaits for a 'hello world' reply
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "network.h"

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	int ret = 0;
//	char s[INET6_ADDRSTRLEN];

	//checks if the port number has been provided
	if (argc != 2)
	{
		fprintf(stderr,"usage: client hostname\n");
		exit(EXIT_FAILURE);
	}

    ret = negociate_socket(argv[1], PORT, &sockfd, CONNECT);
    printf("%d\n", ret);
/*
	//translate IPv4 and IPv6 on the fly depending on the client request
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		  s, sizeof s);
	printf("client: connecting to %s\n", s);
*/
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
