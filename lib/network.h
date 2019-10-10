#ifndef NETWORK_ITLG_INCLUDED
#define NETWORK_ITLG_INCLUDED
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BIND    0x01
#define CONNECT 0x02
#define MULTI   0x04

void *get_in_addr(struct sockaddr *sa);
int negociate_socket(const char* remote_ip, const char* port, int* sockfd, char ACTION);
#endif
