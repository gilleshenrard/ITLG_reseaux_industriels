/*
** server.c
** Waits for clients to connect via a stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 23/11/2019
*/

#include <dirent.h>
#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"
#include "algo.h"
#include "serialisation.h"

void sigchld_handler(int s);
int protSer(int rem_sock, char* dirname, char* rem_ip);
int sendstring(void* pkg, void* sockfd);

int main(int argc, char *argv[])
{
	int loc_socket=0, rem_socket=0;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN]="0", dir[128]="0";

	//checks if the port number has been provided
	if (argc!=3)
	{
		print_error("usage: server port <directory name>");
		exit(EXIT_FAILURE);
	}

	strcpy(dir, argv[2]);

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
    loc_socket = negociate_socket(NULL, argv[1], SOCK_STREAM, MULTI|BIND|LISTEN, print_error);
    if(loc_socket == -1){
        print_error("server: unable to create a socket");
        exit(EXIT_FAILURE);
    }

	print_success("server: setup complete, waiting for connections...");

	// main accept() loop
	while(1)
	{
        //wait for client to request a connection + create connection socket accordingly
        if ((rem_socket = acceptServ(loc_socket, s, sizeof(s))) == -1)
        {
            print_error("server: acceptServ: %s", strerror(errno));
            continue;
        }

		print_neutral("server: %s -> connection received", s);

		//create subprocess for the child request
		switch(fork()){
            case -1: //fork error
                print_error("server: fork: %s", strerror(errno));
                break;

            case 0: //child process
                // child doesn't need the listener in a TCP connection
                close(loc_socket);

                print_neutral("server: %s -> processing request", s);

                //process the request (remote socket if TCP, local socket if UDP)
                if(protSer(rem_socket, dir, s) == -1)
                {
                    print_error("server: unable to process the request from %s", s);
                    exit(EXIT_FAILURE);
                }

                print_success("server: %s -> request processed", s);

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
/*  P : Handles the request received from a child                       */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int protSer(int rem_sock, char* dirname, char* rem_ip){
    meta_t lis = {NULL, 0, FILENAMESZ, compare_dataset};
    DIR *d = NULL;
    struct dirent *dir = NULL;
    unsigned char serialised[MAXDATASIZE] = {0};
    head_t header = {0, FILENAMESZ};
    char filename[64] = "0";
    int ret= 0 ;

    //create a list with all the files in the directory
    if((d = opendir(dirname)) != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
            {
                strcpy(filename, dir->d_name);
                insertListBottom(&lis, filename);
            }
        }
        closedir(d);
    }
    else
    {
        print_error("server: %s -> opendir: %s", rem_ip, strerror(errno));
        return -1;
    }

    //prepare and send the header with the data information
    header.nbelem = lis.nbelements;
    pack(serialised, HEAD_F, header.nbelem, header.szelem);
    ret = sendData(rem_sock, serialised, sizeof(head_t), NULL, 1);
    if (ret == -1)
    {
        print_error("server: %s -> sendData: %s", rem_ip, strerror(errno));
        freeDynList(&lis);
        return -1;
    }

    //send the whole list to the client
    ret = foreachList(&lis, &rem_sock, sendstring);
    freeDynList(&lis);
    if(ret == -1)
    {
        print_error("server: %s -> error while sending the directory list", rem_ip);
        freeDynList(&lis);
        return -1;
    }

    //reset the header and check if it matches the one sent by the client
    memset(serialised, 0, sizeof(serialised));
    receiveData(rem_sock, serialised, sizeof(head_t), NULL, 1);
    unpack(serialised, HEAD_F, &header.nbelem, &header.szelem);
    if(header.nbelem == lis.nbelements && header.szelem == FILENAMESZ)
    {
        print_neutral("server: %s -> acknowledge checks up", rem_ip);
        return 0;
    }
    else
    {
        print_error("server: %s -> client received the wrong information", rem_ip);
        return -1;
    }
}

/************************************************************************/
/*  I : socket file descriptor to which send the string                 */
/*      string to send                                                  */
/*  P : Sends the string to the socket                                  */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int sendstring(void* pkg, void* sockfd)
{
    int* sock = (int*)sockfd;

    if (sendData(*sock, pkg, FILENAMESZ, NULL, 1) == -1)
    {
        print_error("server: sendData: %s", strerror(errno));
        return -1;
    }
    return 0;
}
