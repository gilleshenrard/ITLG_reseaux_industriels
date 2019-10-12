/*
** server.c
** Waits for clients to connect via a stream socket, and sends them a 'Hello World' reply
** Made by Brian 'Beej Jorgensen' Hall
** Modified by Gilles Henrard
*/

#include <time.h>
#include "global.h"
#include "network.h"

void sigchld_handler(/*int s*/);
int process_childrequest(int rem_sock);

int main(int argc, char *argv[])
{
	int loc_socket, rem_socket; // listen on loc_socket, new connection on rem_socket
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	int ret = 0;

	//checks if the port number have been provided
	if (argc != 2)
	{
		fprintf(stderr,"usage: server port\n");
		exit(EXIT_FAILURE);
	}

	//create a local socket and handle any error
    ret = negociate_socket(NULL, argv[1], &loc_socket, MULTI|BIND);
    if(ret != 0){
        if(errno != 0)
            perror("server");
        else
            fprintf(stderr, "server: %s\n", gai_strerror(ret));

        exit(EXIT_FAILURE);
    }

	//listen to socket created
	if (listen(loc_socket, BACKLOG) == -1)
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
		if ((rem_socket = accept(loc_socket, (struct sockaddr *)&their_addr, &sin_size)) == -1)
		{
			perror("accept");
			continue;
		}

		//translate IPv4 and IPv6 on the fly depending on the client request
		socket_to_ip(&rem_socket, s, sizeof(s));
		printf("server: got connection from %s\n", s);

		switch(fork()){
            case -1: //fork error
                perror("fork");
                break;

            case 0: //child process
                close(loc_socket); // child doesn't need the listener

                //process the request
                process_childrequest(rem_socket);

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
/*  I : socket file descriptor of the requesting child                  */
/*  P : Handles the request received from a child                       */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int process_childrequest(int rem_sock){
    time_t timer = {0};
    char buffer[32] = {0};
    struct tm* tm_info = {0};

    //get current time
    time(&timer);
    tm_info = localtime(&timer);

    //format time with day, date, time and time zone
    strftime(buffer, sizeof(buffer), "%a %d-%m-%Y %H:%M:%S %Z", tm_info);

    //send message to child
    if (send(rem_sock, buffer, strlen(buffer), 0) == -1){
        perror("send");
        return -1;
    }

    return 0;
}
