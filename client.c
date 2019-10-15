/*
** client.c
** Connects to a server via a stream/datagram socket and awaits for a reply with the local time
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 13/10/2019
*/

#include "global.h"
#include "network.h"

int main(int argc, char *argv[])
{
    // any IP type, tcp by default, any client's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
    struct addrinfo *servinfo = NULL;
	int sockfd=0, numbytes=0, ret=0;
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	char dummy = '0';

	//checks if the hostname and the port number have been provided
	if (argc!=4)
	{
		fprintf(stderr,"usage: client hostname port tcp|udp\n");
		exit(EXIT_FAILURE);
	}

    //set the socket type to datagram if udp is used
    if(!strcmp(argv[3], "udp"))
    {
        hints.ai_socktype = SOCK_DGRAM;
    }

	//format socket information and store it in list servinfo
	if ((ret = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(ret));
		exit(EXIT_FAILURE);
    }

    //create the actual socket
    //if UDP is chosen, socket will be a connected datagram socket
    //  see pg 32 of Beej's book
    ret = negociate_socket(servinfo, &sockfd, 0, CONNECT);
    if(ret != 0){
        fprintf(stderr, "client: could not create a socket\n");
        exit(EXIT_FAILURE);
    }

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);

    //server info list is not needed anymore
    freeaddrinfo(servinfo);

    if (!strcmp(argv[3], "udp") && send(sockfd, &dummy, 1, 0) == -1)
    {
        perror("client: send");
        exit(EXIT_FAILURE);
    }

    //receive message from the server
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("client: recv");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);

	close(sockfd);
	return 0;
}
