/*
** server.c
** Waits for clients to connect via a stream socket, and sends them a 'Hello World' reply
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "network.h"

#define PORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(/*int s*/);

int main(/*int argc, char *argv[]*/)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	int ret = 0;

    ret = negociate_socket(NULL, PORT, &sockfd, BIND);
    printf("%d\n", ret);

	//listen to socket created
	if (listen(sockfd, BACKLOG) == -1)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	//prepare main process for SIGCHLD signals
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	printf("server: waiting for connections...\n");

	// main accept() loop
	while(1)
	{
        //wait for client to request a connection + create connection socket accordingly
		sin_size = sizeof their_addr;
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
		{
			perror("accept");
			continue;
		}

		//translate IPv4 and IPv6 on the fly depending on the client request
		inet_ntop(their_addr.ss_family,
			  get_in_addr((struct sockaddr *)&their_addr),
			  s, sizeof s);
		printf("server: got connection from %s\n", s);

		switch(fork()){
            case -1: //fork error
                perror("fork");
                break;

            case 0: //child process
                close(sockfd); // child doesn't need the listener

                //send message to child
                if (send(new_fd, "Hello, world!", 13, 0) == -1)
                    perror("send");

                //close connection socket and exit child process
                close(new_fd);
                exit(EXIT_SUCCESS);
                break;

            default: //parent process
                close(new_fd); // parent doesn't need this
                break;
		}
	}

	exit(EXIT_SUCCESS);
}

/************************************************************************/
/*  I : signal number                                                   */
/*  P : Handles the SIGCHLD signal                                      */
/*  O : /                                                               */
/************************************************************************/
void sigchld_handler(/*int s*/)
{
    // waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	printf("Received a SIGCHLD signal...\n");
	while (waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}
