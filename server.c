/*
** server.c
** Waits for clients to connect via a stream socket, wait for a message from a client and echoes it
** -------------------------------------------------------
** Based on Brian 'Beej Jorgensen' Hall's code
** Made by Gilles Henrard
** Last modified : 10/11/2019
*/

#include "global.h"
#include "network.h"
#include "screen.h"
#include "dataset.h"
#include "algo.h"
#include "serialisation.h"

void sigchld_handler(int s);
int process_childrequest(int rem_sock);

int main(int argc, char *argv[])
{
	int loc_socket=0, rem_socket=0;
	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];

	//checks if the port number has been provided
	if (argc != 2)
	{
		print_error("usage: server port");
		exit(EXIT_FAILURE);
	}

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

                //process the request (remote socket if TCP, local socket if UDP)
                if(process_childrequest(rem_socket) == -1)
                {
                    print_error("server: unable to process the request from %s", s);
                    exit(EXIT_FAILURE);
                }

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
int process_childrequest(int rem_sock){
    t_algo_meta ds_list = {NULL, 0, sizeof(dataset_t), compare_dataset_id, swap_dataset, copy_dataset, NULL, NULL, NULL, dataset_right, dataset_left};
    dataset_t tmp = {0};
    char child_addr[INET6_ADDRSTRLEN] = {0};

    //retrieve client's information
    socket_to_ip(&rem_sock, child_addr, sizeof(child_addr));
    print_neutral("server: %s -> processing request", child_addr);

    //fill in 5 dummy elements and add them in a list
    for(int i=1 ; i<6 ; i++)
    {
        tmp.id = i;
        sprintf(tmp.type, "type_%d", i);
        tmp.price = 3.1416*(float)i;

        insertListSorted(&ds_list, &tmp);
    }

    //display all elements in the list, then free it
    foreachList(&ds_list, NULL, Print_dataset);
    while(ds_list.structure)
        popListTop(&ds_list);
/*
    //send message to child
    //wait for a message from the client
    if (receiveData(rem_sock, buffer, MAXDATASIZE-1, NULL, 1) == -1)
    {
        print_error("server: receiveData: %s", strerror(errno));
        return -1;
    }
    print_neutral("server: %s -> sent '%s' (size : %ld)", child_addr, buffer, strlen(buffer));

    //send the reply
    if (sendData(rem_sock, buffer, strlen(buffer), NULL, 1) == -1)
    {
        print_error("server: sendData: %s", strerror(errno));
        return -1;
    }
*/
    print_success("server: %s -> request processed", child_addr);
    return 0;
}
