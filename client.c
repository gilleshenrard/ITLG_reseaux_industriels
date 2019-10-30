/*
** client.c
** Connects to a server via a stream/datagram socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 30/10/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"

void sigalrm_handler(/*int s*/);

int main(int argc, char *argv[])
{
	int sockfd=0, numbytes=0, tcp=TCP;
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	struct sigaction sa = {0};

	//checks if the hostname and the port number have been provided
	if (argc!=4 && argc!=5)
	{
        print_error("usage: client hostname port tcp|udp [message]");
		exit(EXIT_FAILURE);
	}

    //prepare main process for SIGALRM signals
	sa.sa_handler = sigalrm_handler;
	sigemptyset(&sa.sa_mask);
	//sigaddset(&sa.sa_mask, SIGALRM);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
        print_error("client: sigaction: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

    //set the socket type to datagram if udp is used
    if(!strcmp(argv[3], "udp"))
        tcp = UDP;

    //set connection timeout alarm
    alarm(5);

    //create the actual socket
    //if UDP is chosen, socket will be a connected datagram socket
    //  see pg 32 of Beej's book
    sockfd = negociate_socket(argv[1], argv[2], tcp, CONNECT, print_error);
    if(sockfd == -1){
        print_error("client: unable to create a socket");
        exit(EXIT_FAILURE);
    }

    //stop timeout alarm and free the server info list
    alarm(0);

    //make the user type the message if not specified in program argument
    if(argc == 4)
    {
        printf("Entrez le message a envoyer au serveur : ");
        if(fgets(buf, MAXDATASIZE, stdin) == NULL)
        {
            fflush(stdin);
            print_error("client: error while getting the message");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        if(buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';
        fflush(stdin);
    }
    else
    {
        strcpy(buf, argv[4]);
    }

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    printf("client: connecting to %s\n", s);
    printf("client: sending '%s' (size: %ld)\n", buf, strlen(buf));

    //send the message to the server
    if (send(sockfd, buf, strlen(buf), 0) == -1)
    {
        print_error("client: send: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //wipe out the buffer
    memset(buf, 0, MAXDATASIZE);

    //receive message from the server
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        print_error("client: recv: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buf[numbytes] = '\0';
    print_success("client: received '%s' (size: %ld) from the server\n", buf, strlen(buf));

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
    print_error("client: connection attempt timeout");
}
