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
/*      file descriptor of the socket to be created                     */
/*      additional action to perform (catenated with | operator)        */
/*          MULTI   : make the socket able to reconnect if conn. exists */
/*          BIND    : binds the socket to a port or a service           */
/*          CONNECT : initiates a connection on the socket              */
/*  P : creates a socket with the desired values and flags              */
/*  O : on success : 0                                                  */
/*      on error : non-zero value, and either errno is set,             */
/*                      or return value can be tested with gai_strerror */
/************************************************************************/
int negociate_socket(struct addrinfo* sockinfo, int* sockfd, char ACTION){
    struct addrinfo *p=NULL;
    int yes=1;

	//take the first solution available
    for (p = sockinfo; p != NULL; p = p->ai_next)
	{
        //generate a socket file descriptor
		if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
            perror("socket");
			continue;
		}

		//allow reconnections on the socket if still allocated in kernel
		if (ACTION & MULTI){
            if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
            {
                perror("setsockopt");
                continue;
            }
		}

		//bind the socket to the desired port (useful in a server)
		if (ACTION & BIND){
            if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                perror("bind");
                continue;
            }
        }

        //connect to the server via the socket created
        if (ACTION & CONNECT){
            if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1)
            {
                perror("connect");
                continue;
            }
        }

		break;
	}

    //no socket available
	if (p == NULL)
	{
        fprintf(stderr, "no socket available");
		return -1;
    }

	return 0;
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
/*  I : file descriptor of the socket of which to get the IP            */
/*      buffer to fill with the IP address                              */
/*      size of the buffer                                              */
/*  P : recovers the IP address of a corresponding socket, and fills    */
/*          a buffer                                                    */
/*  O : on success : 0                                                  */
/*      on error : -1, and errno is set                                 */
/************************************************************************/
int talk_udp(int* sockfd, struct addrinfo* sockinfo, char* buffer, int buf_len){
    struct addrinfo* p=NULL;
    int numbytes=0;

    for (p = sockinfo; p != NULL; p = p->ai_next)
    {
        printf("sending dummy byte\n");
        if ((numbytes = sendto(*sockfd, "0", 1, 0, (struct sockaddr *)p->ai_addr, p->ai_addrlen)) == -1)
        {
            perror("client: sendto");
            return -1;
        }

        printf("receiving reply\n");
        if ((numbytes = recvfrom(*sockfd, buffer, buf_len, 0, (struct sockaddr *)p->ai_addr, &p->ai_addrlen)) == -1)
        {
            perror("client: recvfrom");
            return -1;
        }
    }

    if(p == NULL){
        fprintf(stderr, "send_udp: no socket available\n");
        return -1;
    }

    return numbytes;
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
int reply_udp(int* sockfd, struct addrinfo* sockinfo, char* buffer, int buf_len){
    int numbytes=0;
    char buf_tmp = '0';

    printf("receiving dummy byte\n");
    if ((numbytes = recvfrom(*sockfd, &buf_tmp, 1, 0, (struct sockaddr *)sockinfo->ai_addr, &sockinfo->ai_addrlen)) == -1)
    {
        perror("client: recvfrom");
        return -1;
    }

    printf("sending reply\n");
    if ((numbytes = sendto(*sockfd, buffer, buf_len, 0, (struct sockaddr *)sockinfo->ai_addr, sockinfo->ai_addrlen)) == -1)
    {
        perror("client: sendto");
        return -1;
    }

    return numbytes;
}
