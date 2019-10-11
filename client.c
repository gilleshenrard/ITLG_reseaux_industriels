/*
** client.c
** Connects to a server via a stream socket and awaits for a 'hello world' reply
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/
#include "global.h"
#include "network.h"

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	int ret = 0;
	char s[INET6_ADDRSTRLEN];

	//checks if the hostname and the port number have been provided
	if (argc != 3)
	{
		fprintf(stderr,"usage: client hostname port\n");
		exit(EXIT_FAILURE);
	}

    ret = negociate_socket(argv[1], argv[2], &sockfd, CONNECT);
    if(ret != 0){
        if(errno != 0)
            perror("client");
        else
            fprintf(stderr, "client: %s\n", gai_strerror(ret));

        exit(EXIT_FAILURE);
    }

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);

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
