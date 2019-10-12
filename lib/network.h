#ifndef NETWORK_ITLG_INCLUDED
#define NETWORK_ITLG_INCLUDED
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define BIND    0x01
#define CONNECT 0x02
#define MULTI   0x04

#define TCP     0x01
#define UDP     0x02

void *get_in_addr(struct sockaddr *sa);
int negociate_socket(struct addrinfo* sockinfo, int* sockfd, char ACTION);
int socket_to_ip(int* fd, char* address, int address_len);
#endif
