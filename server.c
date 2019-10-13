/*
** server.c
** Waits for clients to connect via a datagram/stream socket, and sends them the local time
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 13/10/2019
*/

#include <time.h>
#include "global.h"
#include "network.h"

void sigchld_handler(/*int s*/);
int process_childrequest(int rem_sock, struct sockaddr_storage* their_addr, int tcp);

int main(int argc, char *argv[])
{
    // any IP type, tcp by default, any server's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
    struct addrinfo *servinfo = NULL;         // server address information
	struct sockaddr_storage their_addr = {0}; // client address information
	int loc_socket=0, rem_socket=0, ret=0, tcp=1;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	char disposable = '0';

	//checks if the port number has been provided
	if (argc != 3)
	{
		fprintf(stderr,"usage: server port tcp|udp\n");
		exit(EXIT_FAILURE);
	}

	//set the socket type to datagram if udp is used
	if(!strcmp(argv[2], "udp"))
	{
        hints.ai_socktype = SOCK_DGRAM;
        tcp = 0;
	}

	//prepare main process for SIGCHLD signals
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("server: sigaction");
		exit(EXIT_FAILURE);
	}

	//format socket information and store it in list servinfo
	if ((ret = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(ret));
		exit(EXIT_FAILURE);
    }

	//create a local socket and handle any error
    ret = negociate_socket(servinfo, &loc_socket, MULTI|BIND);
    if(ret != 0){
        fprintf(stderr, "server: could not create a socket\n");
        exit(EXIT_FAILURE);
    }

	//listen to socket created, only if TCP connection
	if (tcp && listen(loc_socket, BACKLOG) == -1)
	{
		perror("server: listen");
		exit(EXIT_FAILURE);
	}

	printf("server: setup complete, waiting for connections...\n");

	// main accept() loop
	while(1)
	{
        if(tcp)
        {
            //TCP connection
            //wait for client to request a connection + create connection socket accordingly
            if ((rem_socket = accept(loc_socket, (struct sockaddr *)&their_addr, &sin_size)) == -1)
            {
                perror("accept");
                continue;
            }
		}
		else
		{
            //UDP connection
            //wait for client to request a connection
            if ((ret = recvfrom(loc_socket, &disposable, 1, 0, (struct sockaddr *)&their_addr, &sin_size)) == -1)
            {
                perror("client: recvfrom");
                continue;
            }
		}

        //retrieve client's IP and store it in a buffer
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("server: got connection from %s\n", s);

		//create subprocess for the child request
		switch(fork()){
            case -1: //fork error
                perror("server: fork");
                break;

            case 0: //child process

                // child doesn't need the listener in a TCP connection
                if(tcp)
                    close(loc_socket);

                //process the request (remote socket if TCP, local socket if UDP)
                if(process_childrequest((tcp ? rem_socket : loc_socket), &their_addr, tcp) == -1)
                {
                    fprintf(stderr, "server: unable to process the request from %s", s);
                    exit(EXIT_FAILURE);
                }

                //close connection socket and exit child process
                close(rem_socket);
                exit(EXIT_SUCCESS);
                break;

            default: //parent process
                close(rem_socket); // parent doesn't need this
                break;
		}
	}

	exit(EXIT_SUCCESS);
}

/************************************************************************/
/*  I : signal number                                                   */
/*  P : Make sure to avoid any zombie child process                     */
/*  O : /                                                               */
/************************************************************************/
void sigchld_handler(/*int s*/)
{
    // waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	//make sure any child process terminating has its ressources released
	while (waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

/************************************************************************/
/*  I : socket file descriptor to which send the reply                  */
/*      information about the client                                    */
/*      flag determining the protocol (TCP or UDP)                      */
/*  P : Handles the request received from a child                       */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int process_childrequest(int rem_sock, struct sockaddr_storage* their_addr, int tcp){
    time_t timer = {0};
    char buffer[32] = {0};
    struct tm* tm_info = {0};

    //get current time, and format it in the buffer
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, sizeof(buffer), "%a %d-%m-%Y %H:%M:%S %Z", tm_info);

    //send message to child
    if(tcp)
    {
        if (send(rem_sock, buffer, strlen(buffer), 0) == -1)
        {
            perror("server: reply");
            return -1;
        }
    }
    else
    {
        if ((sendto(rem_sock, buffer, strlen(buffer), 0, (struct sockaddr *)their_addr, sizeof(struct sockaddr_storage))) == -1)
        {
            perror("server: reply");
            return -1;
        }
    }

    printf("server: request processed\n");
    return 0;
}