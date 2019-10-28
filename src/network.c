/*
** network.c
** Library regrouping network-based functions
** ------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 13/10/2019
*/

#include "network.h"

/************************************************************************/
/*  I : socket                                                          */
/*  P : get sockaddr, IPv4 or IPv6                                      */
/*  O : /                                                               */
/************************************************************************/
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/************************************************************************/
/*  I : local socket information                                        */
/*      size of the backlog (number of remote hosts which can connect)  */
/*      additional action to perform (catenated with | operator)        */
/*          MULTI   : make the socket able to reconnect if conn. exists */
/*          BIND    : binds the socket to a port or a service           */
/*          CONNECT : initiates a connection on the socket              */
/*  P : creates a socket with the desired values and flags              */
/*  O : on success : socket file descriptor                             */
/*      on error : non-zero value, and errno is set                     */
/************************************************************************/
int negociate_socket(struct addrinfo* sockinfo, int sz_backlog, char ACTION){
    struct addrinfo *p=NULL;
    int sockfd=0, yes=1;

	//take the first solution available
    for (p = sockinfo; p != NULL; p = p->ai_next)
	{
        //generate a socket file descriptor
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
            perror("socket");
			continue;
		}

		//allow reconnections on the socket if still allocated in kernel
		if (ACTION & MULTI){
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            {
                perror("setsockopt");
                close(sockfd);
                continue;
            }
		}

		//bind the socket to the desired port (useful in a server)
		if (ACTION & BIND){
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                perror("bind");
                close(sockfd);
                continue;
            }
        }

        //connect to the server via the socket created
        if (ACTION & CONNECT){
            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                perror("connect");
                close(sockfd);
                continue;
            }
        }

		break;
	}

    //no socket available
	if (p == NULL)
	{
        fprintf(stderr, "negociation: no socket available\n");
        close(sockfd);
		return -1;
    }

    //listen to socket created
    if(ACTION & LISTEN)
    {
        if (listen(sockfd, sz_backlog) == -1)
        {
            perror("listen");
            close(sockfd);
            return -1;
        }
    }

	return sockfd;
}

/************************************************************************/
/*  I : file descriptor of the socket of which to get the IP            */
/*      buffer to fill with the IP address                              */
/*      size of the buffer                                              */
/*  P : recovers the IP address of a corresponding socket, and fills    */
/*          a buffer                                                    */
/*  O : on success : 0                                                  */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int socket_to_ip(int* fd, char* address, int address_len)
{
    struct sockaddr_storage addr = {0};
    socklen_t addr_size = sizeof(struct sockaddr_storage);

    //get the host info from the socket
    if ((getpeername(*fd, (struct sockaddr *)&addr, &addr_size)) != 0 )
        return -1;

    //format its IP automatically, whether it's IPv4 or IPv6
    inet_ntop(addr.ss_family, get_in_addr((struct sockaddr *)&addr), address, address_len);

    return 0;
}
