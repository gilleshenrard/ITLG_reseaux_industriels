/*
** server.c
** Waits for clients to connect via a stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 06/12/2019
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
int ser_phase1(int rem_sock, char* dirname, meta_t* lis, char* rem_ip);
int ser_phase2(int rem_sock, char* dirname, meta_t* lis, char* rem_ip);
int ser_phase3(int rem_sock, char* filename, char* rem_ip);
int sendstring(void* pkg, void* sockfd);

int main(int argc, char *argv[])
{
    meta_t lis = {NULL, 0, FILENAMESZ, compare_dataset, print_error};
	int loc_socket=0, rem_socket=0;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN]="0", dir[FILENAMESZ]="0";

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
                if(ser_phase1(rem_socket, dir, &lis, s) == -1)
                {
                    print_error("server: phase1: unable to process the request from %s", s);
                    close(rem_socket);
                    exit(EXIT_FAILURE);
                }

                //process the request (remote socket if TCP, local socket if UDP)
                if(ser_phase2(rem_socket, dir, &lis, s) == -1)
                {
                    print_error("server: phase2: unable to process the request from %s", s);
                    close(rem_socket);
                    exit(EXIT_FAILURE);
                }

                //the list is not needed anymore
                freeDynList(&lis);

                //process the request (remote socket if TCP, local socket if UDP)
                if(ser_phase3(rem_socket, dir, s) == -1)
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
/*      name of the directory to list                                   */
/*      IP address of the client                                        */
/*  P : Handles the phase 1 of a request received from a client         */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase1(int rem_sock, char* dirname, meta_t* lis, char* rem_ip){
    DIR *d = NULL;
    struct dirent *dir = NULL;
    unsigned char serialised[MAXDATASIZE] = {0};
    head_t header = {0, 0, FILENAMESZ};
    int bufsz = 0;

    //create a list with all the files in the directory
    if((d = opendir(dirname)) != NULL)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(insertListSorted(lis, dir->d_name) == -1)
            {
                print_error("server: %s -> insertlistbottom: error", rem_ip);
                return -1;
            }
        }
        closedir(d);
    }
    else
    {
        print_error("server: %s -> opendir %s: %s", rem_ip, dirname, strerror(errno));
        return -1;
    }

    //prepare and send the header with the data information
    header.stype = SLIST;
    header.nbelem = lis->nbelements;
    print_neutral("server: %s -> sending %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
    pack(serialised, HEAD_F, header.nbelem, header.stype, header.szelem);
    bufsz = sizeof(head_t);

    //send the header to the client
    if(sendData(rem_sock, serialised, &bufsz, NULL, 1) == -1)
    {
        print_error("server: %s -> sendData: %s", rem_ip, strerror(errno));
        freeDynList(lis);
        return -1;
    }

    //send the whole list to the client
    if(foreachList(lis, &rem_sock, sendstring) == -1)
    {
        print_error("server: %s -> error while sending the directory list", rem_ip);
        freeDynList(lis);
        return -1;
    }

    //reset the header and receive the client's reply
    memset(serialised, 0, sizeof(serialised));
    receiveData(rem_sock, serialised, sizeof(head_t), NULL, 1);
    unpack(serialised, HEAD_F, &header.nbelem, &header.stype, &header.szelem);

    //check if it matches the one sent by the client
    if(header.szelem != lis->nbelements*lis->elementsize)
    {
        print_error("server: %s -> client received %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
        freeDynList(lis);
        return -1;
    }
    else
        print_neutral("server: %s -> reply header matches up", rem_ip);

    return 0;
}

/************************************************************************/
/*  I : socket file descriptor to which send the reply                  */
/*      name of the directory to list                                   */
/*      IP address of the client                                        */
/*  P : Handles the phase 1 of a request received from a client         */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase2(int rem_sock, char* dirname, meta_t* lis, char* rem_ip)
{
    char filename[FILENAMESZ]={0}, fullpath[FILENAMESZ]={0};
    unsigned char serialised[MAXDATASIZE] = {0};
    int choice=0, bufsz=0;
    head_t header = {0};

    //receive the client's reply
    if (receiveData(rem_sock, &choice, sizeof(int), NULL, 1) == -1)
    {
        print_error("server: %s -> receiveData: %s", strerror(errno));
        return -1;
    }
    print_neutral("server: %s -> client chose %d", rem_ip, choice);

    strcpy(filename, (char*)get_listelem(lis, choice-1));

    //prepare and send the header with the data information
    header.stype = SSTRING;
    header.nbelem = 1;
    header.szelem = strlen(filename);
    print_neutral("server: %s -> sending %d elements of %ld bytes", rem_ip, header.nbelem, header.szelem);
    pack(serialised, HEAD_F, header.nbelem, header.stype, header.szelem);
    bufsz = sizeof(head_t);

    //send the header to the client
    if(sendData(rem_sock, serialised, &bufsz, NULL, 1) == -1)
    {
        print_error("server: %s -> sendData: %s", rem_ip, strerror(errno));
        freeDynList(lis);
        return -1;
    }

    //translate the choice into a filename and send it back
    bufsz = FILENAMESZ;
    if(sendData(rem_sock, filename, &bufsz, NULL, 1) == -1)
    {
        print_error("server: %s -> sendData: %s", strerror(errno));
        return -1;
    }

    sprintf(fullpath, "%s/%s", dirname, filename);
    memcpy(dirname, fullpath, FILENAMESZ);

    return 0;
}

/************************************************************************/
/*  I : socket file descriptor to which send the reply                  */
/*      name of the file to transmit                                    */
/*      IP address of the client                                        */
/*  P : Handles the phase 2 of a request received from a client         */
/*  O : -1 on error                                                     */
/*       0 otherwise                                                    */
/************************************************************************/
int ser_phase3(int rem_sock, char* filename, char* rem_ip)
{
    unsigned char serialised[MAXDATASIZE] = {0};
    head_t header = {0, FILENAMESZ};
    int ret= 0, bufsz = 0, fd = 0;
    uint64_t sent = 0, fsize = 0;

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
    pack(serialised, HEAD_F, header.nbelem, header.stype, header.szelem);
    bufsz = sizeof(head_t);

    //send the header
    ret = sendData(rem_sock, serialised, &bufsz, NULL, 1);
    if (ret == -1)
    {
        print_error("server: %s -> sendData: %s", rem_ip, strerror(errno));
        return -1;
    }

    //send the whole requested file
    while(sent < header.szelem)
    {
        //read the data needed
        ret = read(fd, serialised, sizeof(serialised));
        if (ret == -1)
        {
            print_error("server: %s -> read: %s", rem_ip, strerror(errno));
            close(fd);
            return -1;
        }

        //send the data to the client
        if(sendData(rem_sock, serialised, &ret, NULL, 1) == -1)
        {
            print_error("server: %s -> read: %s", rem_ip, strerror(errno));
            close(fd);
            return -1;
        }

        //wipe the buffer
        memset(serialised, 0, sizeof(serialised));
        sent += ret;
    }

    //receive the client's reply
    memset(serialised, 0, sizeof(serialised));
    receiveData(rem_sock, serialised, sizeof(head_t), NULL, 1);
    unpack(serialised, HEAD_F, &header.nbelem, &header.stype, &header.szelem);

    //check if it matches what was supposed to be received
    if(fsize == header.szelem)
    {
        print_neutral("server: %s -> acknowledge checks up", rem_ip);
        close(fd);
        return 0;
    }
    else
    {
        print_error("server: %s -> client received the wrong information", rem_ip);
        close(fd);
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
    int* sock = (int*)sockfd, bufsz = FILENAMESZ;

    if (sendData(*sock, pkg, &bufsz, NULL, 1) == -1)
    {
        print_error("server: sendData: %s", strerror(errno));
        return -1;
    }
    return 0;
}
