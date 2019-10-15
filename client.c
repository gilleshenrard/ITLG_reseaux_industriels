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

void sigalrm_handler(/*int s*/);

int main(int argc, char *argv[])
{
    // any IP type, tcp by default, any client's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
    struct addrinfo *servinfo = NULL;
	int sockfd=0, numbytes=0, ret=0;
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	struct sigaction sa = {0};
	char dummy = '0';

	//checks if the hostname and the port number have been provided
	if (argc!=4)
	{
		fprintf(stderr,"usage: client hostname port tcp|udp\n");
		exit(EXIT_FAILURE);
	}

    //prepare main process for SIGALRM signals
	sa.sa_handler = sigalrm_handler;
	sigemptyset(&sa.sa_mask);
	//sigaddset(&sa.sa_mask, SIGALRM);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
		perror("client: sigaction");
		exit(EXIT_FAILURE);
	}

    //set the socket type to datagram if udp is used
    if(!strcmp(argv[3], "udp"))
    {
        hints.ai_socktype = SOCK_DGRAM;
    }

    //set connection timeout alarm
    alarm(5);

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

    //stop timeout alarm and free the server info list
    alarm(0);
    freeaddrinfo(servinfo);

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);

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

/************************************************************************/
/*  I : signal number                                                   */
/*  P : Quit program when getting a connection timeout                  */
/*  O : /                                                               */
/************************************************************************/
void sigalrm_handler(/*int s*/)
{
    fprintf(stderr, "client: connection attempt timeout\n");
}
