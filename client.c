/*
** client.c
** Connects to a server via a stream socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 14/11/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"
#include "algo.h"
#include "serialisation.h"

void sigalrm_handler(int s);
int protCli(int sockfd);

int main(int argc, char *argv[])
{
	int sockfd=0;
	struct sigaction sa = {0};
	char s[INET6_ADDRSTRLEN] = {0};

	//checks if the hostname and the port number have been provided
	if (argc!=3)
	{
        print_error("usage: client hostname port");
		exit(EXIT_FAILURE);
	}

    //prepare main process for SIGALRM signals
	sa.sa_handler = sigalrm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
        print_error("client: sigaction: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

    //set connection timeout alarm
    alarm(TIMEOUT);

    //create the actual socket
    sockfd = negociate_socket(argv[1], argv[2], SOCK_STREAM, CONNECT, print_error);
    if(sockfd == -1){
        print_error("client: unable to create a socket");
        exit(EXIT_FAILURE);
    }

    //stop timeout alarm and free the server info list
    alarm(0);

    //notify the successful connection to the server
    socket_to_ip(&sockfd, s, sizeof(s));
    print_neutral("client: connecting to %s", s);

    //handle the protocol on the client side
    if(protCli(sockfd) == -1){
        close(sockfd);
        exit(EXIT_FAILURE);
    }

	close(sockfd);
	exit(EXIT_SUCCESS);
}

/************************************************************************/
/*  I : signal number                                                   */
/*  P : Quit program when getting a connection timeout                  */
/*  O : /                                                               */
/************************************************************************/
void sigalrm_handler(int s)
{
    print_error("client: connection attempt timeout");
}

/************************************************************************/
/*  I : client socket file descriptor                                   */
/*  P : Handle the protocol on the client side                          */
/*  O : 0 if ok                                                         */
/*      -1 otherwise                                                    */
/************************************************************************/
int protCli(int sockfd)
{
    meta_t ds_list = {NULL, 0, sizeof(dataset_t), compare_dataset};
    unsigned char serialised[MAXDATASIZE] = {0};
	dataset_t tmp = {0};
	head_t header = {0};

	//wait for the header containing the data info
    if (receiveData(sockfd, serialised, sizeof(head_t), NULL, 1) == -1)
    {
        print_error("client: receiveData: %s", strerror(errno));
        return -1;
    }
    unpack(serialised, HEAD_F, &header.nbelem, &header.szelem);
    print_neutral("client: will receive %d elements of %d bytes", header.nbelem, header.szelem);
    memset(&serialised, 0, sizeof(serialised));

    //unpack all the data sent by the server and store it in a linked list
    for(int i=0 ; i<header.nbelem ; i++)
    {
        //receive message from the server
        if (receiveData(sockfd, serialised, header.szelem, NULL, 1) == -1)
        {
            print_error("client: receiveData: %s", strerror(errno));
            return -1;
        }

        print_neutral("received : %x", serialised);

        //unpack the data and store it
        unpack(serialised, DATA_F, &tmp.id, tmp.type, &tmp.price);
        insertListSorted(&ds_list, (void*)&tmp);

        //clear the memory buffers
        memset(&serialised, 0, sizeof(serialised));
        memset(&tmp, 0, sizeof(dataset_t));
    }

    //display all elements in the list, then free it
    foreachList(&ds_list, NULL, Print_dataset);
    while(ds_list.structure)
        popListTop(&ds_list);

    //print_success("client: received '%s' (size: %ld) from the server\n", serialised, strlen(serialised));

    return 0;
}
