/*
** client.c
** Connects to a server via a stream/datagram socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 25/10/2019
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
    sockfd = negociate_socket(servinfo, 0, CONNECT);
    if(sockfd == -1){
        fprintf(stderr, "client: could not create a socket\n");
        exit(EXIT_FAILURE);
    }

    //stop timeout alarm and free the server info list
    alarm(0);
    freeaddrinfo(servinfo);

    printf("Entrez le message a envoyer au serveur : ");
    if(fgets(buf, MAXDATASIZE, stdin) == NULL)
    {
        fflush(stdin);
        fprintf(stderr, "client: error while getting the message\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    if(buf[strlen(buf)-1] == '\n')
        buf[strlen(buf)-1] = '\0';
    fflush(stdin);

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);
    printf("client: sending '%s' (size: %ld)\n", buf, strlen(buf));

    //send the message to the server
    if (send(sockfd, buf, strlen(buf), 0) == -1)
    {
        perror("client: send");
        exit(EXIT_FAILURE);
    }

    //wipe out the buffer
    memset(buf, 0, MAXDATASIZE);

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
	exit(EXIT_SUCCESS);
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
