/*
** client.c
** Connects to a server via a stream socket and awaits for a reply with the local time
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/
#include "global.h"
#include "network.h"

int main(int argc, char *argv[])
{
    // any IP type, tcp by default, server's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
    struct addrinfo *servinfo=NULL;
	int sockfd=0, numbytes=0, ret=0;
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	char dummy = '0';

	//checks if the hostname and the port number have been provided
	if (argc!=3 && argc!=4)
	{
		fprintf(stderr,"usage: client hostname port [udp]\n");
		exit(EXIT_FAILURE);
	}

    //if specified, change the protocol to UDP
    if(argc == 4 && !strcmp(argv[3], "udp"))
    {
        hints.ai_socktype = SOCK_DGRAM;
    }

	//format socket information and store it in list servinfo
	if ((ret = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
    }

    //create the actual socket
    ret = negociate_socket(servinfo, &sockfd, CONNECT);
    if(ret != 0){
        fprintf(stderr, "client: could not create a socket\n");
        exit(EXIT_FAILURE);
    }

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);

    //server info list is not needed anymore
    freeaddrinfo(servinfo);

    if (send(sockfd, &dummy, 1, 0) == -1)
    {
        perror("client: send");
        return -1;
    }

    //receive message from the server
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);

	close(sockfd);
	return 0;
}
