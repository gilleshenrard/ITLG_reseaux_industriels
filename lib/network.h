#ifndef NETWORK_ITLG_INCLUDED
#define NETWORK_ITLG_INCLUDED
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BIND    0
#define CONNECT 1
#define NONE    2

void *get_in_addr(struct sockaddr *sa);
int negociate_socket(const char* remote_ip, const char* port, struct addrinfo* final_socket, int ACTION);
#endif
