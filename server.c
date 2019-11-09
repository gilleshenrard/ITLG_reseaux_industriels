/*
** server.c
** Waits for clients to connect via a datagram/stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 09/11/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"

void sigchld_handler(int s);
int process_childrequest(int rem_sock, struct sockaddr_storage* their_addr, int tcp, char* buffer);

int main(int argc, char *argv[])
{
	struct sockaddr_storage their_addr = {0}; // client address information
	int loc_socket=0, rem_socket=0, tcp=1, socktype = SOCK_STREAM;
	socklen_t sin_size = sizeof(struct sockaddr_storage);
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	char buff_rcv[MAXDATASIZE];
	char actions = '0';

	//checks if the port number has been provided
	if (argc != 3)
	{
		print_error("usage: server port tcp|udp");
		exit(EXIT_FAILURE);
	}

	//set the socket type to datagram if udp is used
	if(!strcmp(argv[2], "udp")){
        tcp = 0;
        socktype = SOCK_DGRAM;
    }

	//prepare main process for SIGCHLD signals
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		print_error("server: sigaction: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//create a local socket and handle any error
	actions = (tcp ? MULTI|BIND|LISTEN : MULTI|BIND);
    loc_socket = negociate_socket(NULL, argv[1], socktype, actions, print_error);
    if(loc_socket == -1){
        print_error("server: unable to create a socket");
        exit(EXIT_FAILURE);
    }

	print_success("server: setup complete, waiting for connections...");

	// main accept() loop
	while(1)
	{
        if(tcp)
        {
            //TCP connection
            //wait for client to request a connection + create connection socket accordingly
            if ((rem_socket = acceptServ(loc_socket, s, sizeof(s))) == -1)
            {
                print_error("server: accept: %s", strerror(errno));
                continue;
            }
		}
		else
		{
            //UDP connection
            //wait for client to request a connection
            if (recvfrom(loc_socket, &buff_rcv, MAXDATASIZE, 0, (struct sockaddr *)&their_addr, &sin_size) == -1)
            {
                print_error("server: recvfrom: %s", strerror(errno));
                continue;
            }

            print_neutral("server: client sent '%s'", buff_rcv);
		}

        //retrieve client's IP and store it in a buffer
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		print_neutral("server: %s -> connection received", s);

		//create subprocess for the child request
		switch(fork()){
            case -1: //fork error
                print_error("server: fork: %s", strerror(errno));
                break;

            case 0: //child process

                // child doesn't need the listener in a TCP connection
                if(tcp)
                    close(loc_socket);

                //process the request (remote socket if TCP, local socket if UDP)
                if(process_childrequest((tcp ? rem_socket : loc_socket), &their_addr, tcp, buff_rcv) == -1)
                {
                    print_error("server: unable to process the request from %s", s);
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
void sigchld_handler(int s)
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
    print_neutral("server: %s -> processing request", child_addr);

    //send message to child
    if(tcp)
    {
        //wait for a message from the client
        if ((numbytes = recv(rem_sock, buffer, MAXDATASIZE-1, 0)) == -1)
        {
            print_error("server: recv: %s", strerror(errno));
            return -1;
        }
		print_neutral("server: %s -> sent '%s' (size : %ld)", child_addr, buffer, strlen(buffer));

		//send the reply
        if (send(rem_sock, buffer, strlen(buffer), 0) == -1)
        {
            print_error("server: send: %s", strerror(errno));
            return -1;
        }
    }
    else
    {
        //send the reply
        if ((sendto(rem_sock, buffer, strlen(buffer), 0, (struct sockaddr *)their_addr, sizeof(struct sockaddr_storage))) == -1)
        {
            print_error("server: sendto: %s", strerror(errno));
            return -1;
        }
    }

    //wipe out the buffer
    memset(buffer, 0, MAXDATASIZE);
    print_success("server: %s -> request processed", child_addr);
    return 0;
}
