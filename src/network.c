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
/*  I : remote IP or hostname (other host)                              */
/*      remote host's port or service name                              */
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
int negociate_socket(const char* host, const char* port, int* sockfd, char ACTION){
    struct addrinfo hints={0}, *servinfo=NULL, *p=NULL;
    int rv=0, yes=1;

    //prepare the structure holding socket information
	// any potocol, stream socket, remote IP if any, rest to 0
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(!host)
        hints.ai_flags = AI_PASSIVE; // use my IP

	//format socket information and store it in list servinfo
	if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
    }

	//take the first solution available
    for (p = servinfo; p != NULL; p = p->ai_next)
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

    // no need for the serv structure list anymore
	freeaddrinfo(servinfo);

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
