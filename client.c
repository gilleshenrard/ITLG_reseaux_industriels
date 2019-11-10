/*
** client.c
** Connects to a server via a stream socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 10/11/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"

void sigalrm_handler(int s);

int main(int argc, char *argv[])
{
	int sockfd=0;
	char buf[MAXDATASIZE] = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	struct sigaction sa = {0};

	//checks if the hostname and the port number have been provided
	if (argc!=3 && argc!=4)
	{
        print_error("usage: client hostname port [message]");
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

    //set connection timeout alarm
    alarm(TIMEOUT);

    //create the actual socket
    sockfd = negociate_socket(argv[1], argv[2], SOCK_STREAM, CONNECT, print_error);
    if(sockfd == -1){
        print_error("client: unable to create a socket");
        exit(EXIT_FAILURE);
    }

    //stop timeout alarm and free the server info list
    alarm(0);

    //make the user type the message if not specified in program argument
    if(argc == 3)
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
        strcpy(buf, argv[3]);
    }

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    print_neutral("client: connecting to %s", s);
    print_neutral("client: sending '%s' (size: %ld)", buf, strlen(buf));

    //send the message to the server
    if (sendData(sockfd, buf, strlen(buf), NULL, 1) == -1)
    {
        print_error("client: send: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //wipe out the buffer
    memset(buf, 0, MAXDATASIZE);

    //receive message from the server
    if (receiveData(sockfd, buf, MAXDATASIZE-1, NULL, 1) == -1)
    {
        print_error("client: receiveData: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    print_success("client: received '%s' (size: %ld) from the server\n", buf, strlen(buf));

	close(sockfd);
	exit(EXIT_SUCCESS);
}

/************************************************************************/
/*  I : signal number                                                   */
/*  P : Quit program when getting a connection timeout                  */
/*  O : /                                                               */
/************************************************************************/
void sigalrm_handler(int s)
{
    print_error("client: connection attempt timeout");
}
