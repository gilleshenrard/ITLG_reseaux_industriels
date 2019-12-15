/*
** server.c
** Waits for clients to connect via a stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 15/12/2019
*/
#include <dirent.h>
#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"
#include "algo.h"
#include "serialisation.h"
#include "protocol.h"

void sigchld_handler(int s);
int ser_phase1(int rem_sock, meta_t* lis, char* rem_ip);
int ser_phase2(int rem_sock, char* dirname, meta_t* lis, char* rem_ip);
int ser_phase3(int rem_sock, char* filename, char* rem_ip);
int sendstring(void* pkg, void* sockfd);

int main(int argc, char *argv[])
{
    DIR *d = NULL;
    struct dirent *dir = NULL;
    meta_t lis = {NULL, 0, FILENAMESZ, compare_dataset, print_error};
	int loc_socket=0, rem_socket=0;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN]="0", dirname[FILENAMESZ]="0";

	//checks if the port number and directory path has been provided
	if (argc!=3)
	{
		print_error("usage: server port [directory name]");
		exit(EXIT_FAILURE);
	}

    //create a list with all the files in the directory
    if((d = opendir(argv[2])) != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(insertListSorted(&lis, dir->d_name) == -1)
            {
                print_error("server: insertlistbottom: error");
                exit(EXIT_FAILURE);
            }
        }
        closedir(d);
    }
    else
    {
        print_error("server: opendir %s: %s", dirname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    //copy the directory path in a buffer
	strcpy(dirname, argv[2]);

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

                //process the phase 1 : sending the files list to the client
                if(ser_phase1(rem_socket, &lis, s) == -1)
                {
                    print_error("server: phase1: unable to process the request from %s", s);
                    close(rem_socket);
                    exit(EXIT_FAILURE);
                }

                //process the phase 2 : receiving the client's choice (update dirname)
                if(ser_phase2(rem_socket, dirname, &lis, s) == -1)
                {
                    print_error("server: phase2: unable to process the request from %s", s);
                    close(rem_socket);
                    exit(EXIT_FAILURE);
                }

                //the list is not needed anymore
                freeDynList(&lis);

                //process the phase 3 : sending the file chosen by the client
                if(ser_phase3(rem_socket, dirname, s) == -1)
                {
                    print_error("server: phase3: unable to process the request from %s", s);
                    close(rem_socket);
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
/*      list of the files in te directoty set in program argument       */
/*      IP address of the client                                        */
/*  P : Handles the phase 1: send the files list to the client          */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase1(int rem_sock, meta_t* lis, char* rem_ip)
{
    head_t header = {0, 0, FILENAMESZ};

    //prepare and send the header with the data information
    header.stype = SLIST;
    header.nbelem = lis->nbelements;
    print_neutral("server: %s -> sending %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
    if(psnd(rem_sock, lis, &header, sendstring) == -1)
    {
        print_error("server: %s -> error while sending the list to the client", rem_ip);
        return -1;
    }

    return 0;
}

/************************************************************************/
/*  I : socket file descriptor to which send the reply                  */
/*      name of the file chosen by the client                           */
/*      list of files in the directory set in program argument          */
/*      IP address of the client                                        */
/*  P : Handles the phase 2: wait for the client's choice and update    */
/*          the full path of the file to send                           */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase2(int rem_sock, char* dirname, meta_t* lis, char* rem_ip)
{
    char filename[FILENAMESZ]={0}, fullpath[FILENAMESZ]={0};
    int choice=0;
    head_t header = {0};

    //receive the client's reply
    if (receiveData(rem_sock, &choice, sizeof(int), NULL, 1) == -1)
    {
        print_error("server: %s -> receiveData: %s", strerror(errno));
        return -1;
    }
    print_neutral("server: %s -> client chose %d", rem_ip, choice);

    //interpret the choice number to a filename
    strcpy(filename, (char*)get_listelem(lis, choice-1));

    //prepare and send the header with the data information
    header.stype = SSTRING;
    header.nbelem = 1;
    header.szelem = strlen(filename);
    print_neutral("server: %s -> sending %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
    if(psnd(rem_sock, filename, &header, NULL) == -1)
    {
        print_error("server: %s -> error while sending the filename to the client", rem_ip);
        return -1;
    }

    //update the full path
    sprintf(fullpath, "%s/%s", dirname, filename);
    memcpy(dirname, fullpath, FILENAMESZ);

    return 0;
}

/************************************************************************/
/*  I : socket file descriptor to which send the reply                  */
/*      name of the file to transmit                                    */
/*      IP address of the client                                        */
/*  P : Handles the phase 3: sending the file to the client             */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase3(int rem_sock, char* filename, char* rem_ip)
{
    head_t header = {0, FILENAMESZ};
    int fd = 0;
    uint64_t fsize = 0;

    //open the requested file
    if((fd = open(filename, O_RDONLY)) == -1)
    {
        print_error("server: %s -> open: %s", strerror(errno));
        return -1;
    }
    fsize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    //prepare the header with the data information
    header.szelem = fsize;
    header.nbelem = 1;
    header.stype = SFILE;
    print_neutral("server: %s -> sending %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
    if(psnd(rem_sock, &fd, &header, NULL))
    {
        print_error("server: %s -> error while sending the file to the client", rem_ip);
        return -1;
    }

    return 0;
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
    int* sock = (int*)sockfd, bufsz = FILENAMESZ;

    if (sendData(*sock, pkg, &bufsz, NULL, 1) == -1)
    {
        print_error("server: sendData: %s", strerror(errno));
        return -1;
    }
    return 0;
}
