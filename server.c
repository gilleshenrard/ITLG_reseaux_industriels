/*
** server.c
** Waits for clients to connect via a datagram/stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 29/10/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"

void sigchld_handler(/*int s*/);
int process_childrequest(int rem_sock, struct sockaddr_storage* their_addr, int tcp, char* buffer);

int main(int argc, char *argv[])
{
    // any IP type, tcp by default, any server's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0, 0, NULL, NULL, NULL};
    struct addrinfo *servinfo = NULL, *p = NULL;         // server address information
	struct sockaddr_storage their_addr = {0}; // client address information
	int loc_socket=0, rem_socket=0, ret=0, tcp=1;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	char buff_rcv[MAXDATASIZE];
	char actions = '0';

	//checks if the port number has been provided
	if (argc != 3)
	{
		print_error("usage: server port tcp|udp", 0);
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
	actions = (tcp ? MULTI|BIND|LISTEN : MULTI|BIND);
	for (p = servinfo; p != NULL; p = p->ai_next){
        loc_socket = negociate_socket(p, BACKLOG, actions);
        if(loc_socket == -1){
            print_error("server: negotiate_socket", 1);
            //perror("server: negociate_socket");
            continue;
        }
        break;
    }

    //no socket available
	if (p == NULL)
	{
        fprintf(stderr, "negociation: no socket available\n");
        close(loc_socket);
		exit(EXIT_FAILURE);
    }

    //server info list is not needed anymore
    freeaddrinfo(servinfo);

	print_success("server: setup complete, waiting for connections...");

	// main accept() loop
	while(1)
	{
        if(tcp)
        {
            //TCP connection
            //wait for client to request a connection + create connection socket accordingly
            if ((rem_socket = accept(loc_socket, (struct sockaddr *)&their_addr, &sin_size)) == -1)
            {
                perror("server: accept");
                continue;
            }
		}
		else
		{
            //UDP connection
            //wait for client to request a connection
            if ((ret = recvfrom(loc_socket, &buff_rcv, MAXDATASIZE, 0, (struct sockaddr *)&their_addr, &sin_size)) == -1)
            {
                perror("server: recvfrom");
                continue;
            }

            printf("server: client sent '%s'\n", buff_rcv);
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
                if(process_childrequest((tcp ? rem_socket : loc_socket), &their_addr, tcp, buff_rcv) == -1)
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
/*      buffer to send back to the client                               */
/*  P : Handles the request received from a child                       */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int process_childrequest(int rem_sock, struct sockaddr_storage* their_addr, int tcp, char* buffer){
    int numbytes = 0;
    char child_addr[INET6_ADDRSTRLEN] = {0};

    //retrieve client's information
    inet_ntop(their_addr->ss_family, get_in_addr((struct sockaddr *)their_addr), child_addr, sizeof child_addr);
    printf("server: %s -> processing request\n", child_addr);

    //send message to child
    if(tcp)
    {
        //wait for a message from the client
        if ((numbytes = recv(rem_sock, buffer, MAXDATASIZE-1, 0)) == -1)
        {
            perror("server: request");
            return -1;
        }
		printf("server: %s -> sent '%s' (size : %ld)\n", child_addr, buffer, strlen(buffer));

		//send the reply
        if (send(rem_sock, buffer, strlen(buffer), 0) == -1)
        {
            perror("server: reply");
            return -1;
        }
    }
    else
    {
        //send the reply
        if ((sendto(rem_sock, buffer, strlen(buffer), 0, (struct sockaddr *)their_addr, sizeof(struct sockaddr_storage))) == -1)
        {
            perror("server: reply");
            return -1;
        }
    }

    //wipe out the buffer
    memset(buffer, 0, MAXDATASIZE);
    printf("server: %s -> request processed\n", child_addr);
    return 0;
}
