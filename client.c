/*
** client.c
** Connects to a server via a stream socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 23/11/2019
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
    meta_t ds_list = {NULL, 0, FILENAMESZ, compare_dataset};
    char buffer[FILENAME_MAX] = {0};
    unsigned char serialised[MAXDATASIZE] = {0};
	head_t header = {0};
	int index = 1, bufsz = 0, fd = 0, ret = 0;
	uint64_t received = 0;

	//wait for the header containing the data info
    if (receiveData(sockfd, serialised, sizeof(head_t), NULL, 1) == -1)
    {
        print_error("client: receiveData: %s", strerror(errno));
        return -1;
    }
    unpack(serialised, HEAD_F, &header.nbelem, &header.szelem);
    print_neutral("client: will receive %d elements of %ld bytes", header.nbelem, header.szelem);
    memset(&serialised, 0, sizeof(serialised));

    //unpack all the data sent by the server and store it in a linked list
    for(int i=0 ; i<header.nbelem ; i++)
    {
        //receive message from the server
        if (receiveData(sockfd, buffer, header.szelem, NULL, 1) == -1)
        {
            print_error("client: receiveData: %s", strerror(errno));
            return -1;
        }

        //unpack the data and store it
        insertListSorted(&ds_list, buffer);

        //clear the memory buffers
        memset(&buffer, 0, sizeof(serialised));
    }

    header.nbelem = ds_list.nbelements;
    header.szelem = ds_list.elementsize;

    memset(serialised, 0, sizeof(serialised));
    pack(serialised, HEAD_F, header.nbelem, header.szelem);
    bufsz = sizeof(head_t);
    sendData(sockfd, serialised, &bufsz, NULL, 1);

    //display all elements in the list, then free it
    foreachList(&ds_list, &index, printdatasetnum);
    freeDynList(&ds_list);

    if((fd = open("data/output.mp3", O_WRONLY|O_CREAT)) == -1)
    {
        print_error("client: open: %s", strerror(errno));
        return -1;
    }

    	//wait for the header containing the data info
    if (receiveData(sockfd, serialised, sizeof(head_t), NULL, 1) == -1)
    {
        print_error("client: receiveData: %s", strerror(errno));
        return -1;
    }
    unpack(serialised, HEAD_F, &header.nbelem, &header.szelem);
    print_neutral("client: will receive %d elements of %d bytes", header.nbelem, header.szelem);

    //unpack all the data sent by the server and store it in a linked list
    while(received < header.szelem)
    {
        memset(&serialised, 0, sizeof(serialised));

        //receive message from the server
        if ((ret = receiveData(sockfd, serialised, MAXDATASIZE, NULL, 1)) == -1)
        {
            print_error("client: receiveData: %s", strerror(errno));
            return -1;
        }

        //unpack the data and store it
        if((ret = write(fd, serialised, ret)) == -1)
        {
            print_error("client: write: %s", strerror(errno));
            close(fd);
            return -1;
        }

        //clear the memory buffers
        received += ret;
    }
    close(fd);

    header.nbelem = 1;
    header.szelem = received;

    memset(serialised, 0, sizeof(serialised));
    pack(serialised, HEAD_F, header.nbelem, header.szelem);
    bufsz = sizeof(head_t);
    sendData(sockfd, serialised, &bufsz, NULL, 1);

    return 0;
}
