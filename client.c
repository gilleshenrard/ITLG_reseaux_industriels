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
    struct addrinfo hints={0}, *servinfo=NULL;
	int sockfd=0, numbytes=0, ret=0;
	char buf[MAXDATASIZE];
	char s[INET6_ADDRSTRLEN];

	//checks if the hostname and the port number have been provided
	if (argc != 3)
	{
		fprintf(stderr,"usage: client hostname port\n");
		exit(EXIT_FAILURE);
	}

    //prepare the structure holding socket information
	// any IP type, connection protocol, remote IP if any, rest to 0
	hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

	//format socket information and store it in list servinfo
	if ((ret = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
    }

    ret = negociate_socket(servinfo, &sockfd, CONNECT);
    if(ret != 0){
        fprintf(stderr, "client: could not create a socket\n");
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
