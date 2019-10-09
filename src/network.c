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
/*      pointer to the final socket created
/*  P : get sockaddr, IPv4 or IPv6                                      */
/*  O : /                                                               */
/************************************************************************/
int negociate_socket(const char* remote_ip, const char* port, struct addrinfo* final_socket, int ACTION){
    struct addrinfo hints={0}, *servinfo=NULL;
    int sockfd=0, rv=0, yes=1;

    //prepare the structure holding socket information
	// any potocol, stream socket, remote IP if any, rest to 0
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if(!remote_ip)
        hints.ai_flags = AI_PASSIVE; // use my IP

	//format socket information and store it in list servinfo
	if ((rv = getaddrinfo(remote_ip, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return rv;
	}

    for (final_socket = servinfo; final_socket != NULL; final_socket = final_socket->ai_next)
	{
        //generate a socket file descriptor
		if ((sockfd = socket(final_socket->ai_family, final_socket->ai_socktype, final_socket->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

        //allow reconnections on the socket if still allocated in kernel
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}

		switch(ACTION){
            case BIND:      //bind the socket to the desired port (useful in a server)
                if (bind(sockfd, final_socket->ai_addr, final_socket->ai_addrlen) == -1)
                {
                    close(sockfd);
                    perror("server: bind");
                    continue;
                }
                break;

            case CONNECT:   //connect to the server via the socket created
                if (connect(sockfd, final_socket->ai_addr, final_socket->ai_addrlen) == -1)
                {
                    close(sockfd);
                    perror("client: connect");
                    continue;
                }
                break;

            default:
                break;
		}

		break;
	}

	// no need for the serv structure list anymore
	freeaddrinfo(servinfo);

    //no socket available
	if (final_socket == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	return 0;
}
