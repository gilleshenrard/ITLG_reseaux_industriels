/*
** network.c
** Library regrouping network-based functions
** ------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 29/11/2019
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
/*  I : host to link to the socket                                      */
/*      service to link to the socket                                   */
/*      socket type (SOCK_STREAM or SOCK_DGRAM)                         */
/*      additional action to perform (catenated with | operator)        */
/*          MULTI   : make the socket able to reconnect if conn. exists */
/*          BIND    : binds the socket to a port or a service           */
/*          CONNECT : initiates a connection on the socket              */
/*          LISTEN  : listens to any connection on the specified port   */
/*      function to print error messages (if NULL, default output)      */
/*  P : creates a socket with the desired values (100 clients max)      */
/*  O : on success : socket file descriptor                             */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int negociate_socket(char* host, char* service, int socktype, char ACTION, void (*on_error)(char*, ...)){
    // any IP type, tcp by default, any server's IP
    struct addrinfo hints={AI_PASSIVE, AF_UNSPEC, socktype, 0, 0, NULL, NULL, NULL};
    struct addrinfo *p = NULL, *servinfo = NULL;
    int sockfd = 0, yes=1, ret=0;

    //format socket information and store it in list servinfo
	if ((ret = getaddrinfo(host, service , &hints, &servinfo)) != 0)
	{
        if(on_error != NULL)
            (*on_error)("getaddrinfo: %s", gai_strerror(ret));
        else
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		return -1;
    }

    //find the first socket available on all net interfaces
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        //generate a socket file descriptor
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            if(on_error != NULL)
                (*on_error)("socket: %s", strerror(errno));
            else
                perror("socket");
            continue;
        }

        //allow reconnections on the socket if still allocated in kernel
        if (ACTION & MULTI){
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
                if(on_error != NULL)
                    (*on_error)("setsockopt: %s", strerror(errno));
                else
                    perror("setsockopt");
                close(sockfd);
                continue;
            }
        }

        //bind the socket to the desired port (useful in a server)
        if (ACTION & BIND){
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
                if(on_error != NULL)
                    (*on_error)("bind: %s", strerror(errno));
                else
                    perror("bind");
                close(sockfd);
                continue;
            }
        }

        //connect to the server via the socket created
        if (ACTION & CONNECT){
            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
                if(on_error != NULL)
                    (*on_error)("connect: %s", strerror(errno));
                else
                    perror("connect");
                close(sockfd);
                continue;
            }
        }

        break;
    }

    //socket info list is not needed anymore
    freeaddrinfo(servinfo);

    //no socket available
	if (p == NULL)
	{
        if(on_error != NULL)
            (*on_error)("negociation: no socket available");
        else
            fprintf(stderr, "negociation: no socket available\n");
        close(sockfd);
		return -1;
    }

    //listen to socket created
    if(ACTION & LISTEN)
    {
        if (listen(sockfd, BACKLOG) == -1){
            if(on_error != NULL)
                (*on_error)("listen: %s", strerror(errno));
            else
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

/************************************************************************/
/*  I : file descriptor of the server socket                            */
/*      buffer to fill with the IP address                              */
/*      size of the buffer                                              */
/*  P : Accept the connection of a client on the TCP socket,            */
/*          fills the buffer with its IP, and returns its socket f. des.*/
/*  O : on success : client socket file descriptor                      */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int acceptServ(int sockfd, char* client, int ip_size)
{
    int cli_sockfd = 0;
    struct sockaddr_storage their_addr = {0};
    socklen_t sin_size = sizeof(struct sockaddr_storage);

    //wait for a client connection on the server TCP socket
    if ((cli_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        return -1;

    //translate the client IP and feed it in the client buffer
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), client, ip_size);

    return cli_sockfd;
}

/************************************************************************/
/*  I : file descriptor of the socket on which read the data            */
/*      buffer to fill with the data received                           */
/*      size of the data buffer                                         */
/*      buffer to fill with the IP address                              */
/*      flag to indicate whether the socket is connected or not         */
/*  P : Reads data sent by the client, fills up the data buffer and the */
/*          IP buffer                                                   */
/*  O : on success : number of bytes received                           */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int receiveData(int sockfd, void* buf, int bufsz, struct sockaddr_storage* client, int connected)
{
    int numbytes = 0;
    socklen_t size = sizeof(struct sockaddr_storage);

    if(connected)
    {
        //wait data on a connected socket
        if ((numbytes = recv(sockfd, buf, bufsz, 0)) == -1)
            return -1;
    }
    else
    {
        //wait data on a non-connected socket
        if ((numbytes = recvfrom(sockfd, buf, bufsz, 0, (struct sockaddr *)&client, &size)) == -1)
            return -1;
    }

    return numbytes;
}

/************************************************************************/
/*  I : file descriptor of the socket on which read the data            */
/*      buffer to fill with the data received                           */
/*      size of the data buffer                                         */
/*      buffer to fill with the IP address                              */
/*      flag to indicate whether the socket is connected or not         */
/*  P : Sends data to the client                                        */
/*  O : on success : number of bytes sent                               */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int sendData(int sockfd, void* buf, int* length, struct sockaddr_storage* client, int connected)
{
    int numbytes = 0, total = 0, bytesleft = *length;
    socklen_t size = sizeof(struct sockaddr_storage);

    while(total < *length && numbytes >= 0)
    {
        if(connected)
            //send as much data as possible to the socket
            numbytes = send(sockfd, buf+total, bytesleft, 0);
        else
            //send as much data as possible to the socket
            numbytes = sendto(sockfd, buf+total, bytesleft, 0, (struct sockaddr *)client, size);

        //if ok, update the amount of data send and the amount still to be sent
        if(numbytes != -1)
        {
            total += numbytes;
            bytesleft -= numbytes;
        }
    }

    //update the total of bytes sent
    *length = total;

    return numbytes;
}
