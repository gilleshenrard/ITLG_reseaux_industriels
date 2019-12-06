/*
** client.c
** Connects to a server via a stream socket, asks for a message and sends it to the echo server
** -------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 06/12/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"
#include "algo.h"
#include "serialisation.h"
#include "protocol.h"

void sigalrm_handler(int s);
int cli_phase1(int sockfd);
int cli_phase2(int sockfd, char* filename);
int cli_phase3(int sockfd, char* filename);

int main(int argc, char *argv[])
{
	int sockfd=0;
	struct sigaction sa = {0};
	char s[INET6_ADDRSTRLEN] = {0};
	char filename[FILENAMESZ] = "0";

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
    if(cli_phase1(sockfd) == -1){
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //handle the protocol on the client side
    if(cli_phase2(sockfd, filename) == -1){
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //handle the protocol on the client side
    if(cli_phase3(sockfd, filename) == -1){
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
/*  P : Handle the phase 1 of a request to the server                   */
/*  O : 0 if ok                                                         */
/*      -1 otherwise                                                    */
/************************************************************************/
int cli_phase1(int sockfd)
{
    meta_t ds_list = {NULL, 0, FILENAMESZ, compare_dataset, print_error};
	int index = 1;

	if(prcv(sockfd, &ds_list) == -1)
        return -1;

    //display all elements in the list, then free it
    foreachList(&ds_list, &index, printdatasetnum);
    freeDynList(&ds_list);

    return 0;
}

/************************************************************************/
/*  I : client socket file descriptor                                   */
/*  P : Handle the phase 1 of a request to the server                   */
/*  O : 0 if ok                                                         */
/*      -1 otherwise                                                    */
/************************************************************************/
int cli_phase2(int sockfd, char* filename)
{
    char buffer[FILENAMESZ] = {0};
	int bufsz = 0, choice = 0;

	//collect the user's choice
    printf("Choose which file to download: ");
    if(fgets(buffer, FILENAMESZ, stdin) == NULL)
    {
        print_error("client: fgets: error while reading the user's choice");
        return -1;
    }
    printf("\n");
    fflush(stdin);
    buffer[strlen(buffer)]='\0';

    //prepare the reply
    choice = atoi(buffer);
    bufsz = sizeof(choice);

    //send it to the server
    if(sendData(sockfd, &choice, &bufsz, NULL, 1) == -1)
    {
        print_error("client: sendData: %s", strerror(errno));
        return -1;
    }

    //receive the file name
    prcv(sockfd, filename);
    printf("filename: %s\n", filename);

    return 0;
}

/************************************************************************/
/*  I : client socket file descriptor                                   */
/*  P : Handle the phase 2 of a request to the server                   */
/*  O : 0 if ok                                                         */
/*      -1 otherwise                                                    */
/************************************************************************/
int cli_phase3(int sockfd, char* filename)
{
    char buffer[FILENAMESZ] = "0";
	int fd = 0;

	//open the soon to be file
	//strcpy(buffer, "data/output.mp3");
	sprintf(buffer, "data/%s", filename);
    if((fd = open(buffer, O_WRONLY|O_CREAT)) == -1)
    {
        print_error("client: open: %s", strerror(errno));
        return -1;
    }
    memset(buffer, 0, MAXDATASIZE);

    prcv(sockfd, &fd);

    return 0;
}
