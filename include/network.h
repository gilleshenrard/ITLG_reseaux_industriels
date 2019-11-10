#ifndef NETWORK_ITLG_INCLUDED
#define NETWORK_ITLG_INCLUDED
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define NONE    0x00
#define BIND    0x01
#define CONNECT 0x02
#define MULTI   0x04
#define LISTEN  0x08

#define BACKLOG 10 // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa);
int negociate_socket(char* host, char* service, int socktype, char ACTION, void (*on_error)(char*, ...));
int socket_to_ip(int* fd, char* address, int address_len);
int acceptServ(int sockfd, char* client, int ip_size);
int receiveData(int sockfd, void* buf, int bufsz, struct sockaddr_storage* client, int connected);
int sendData(int sockfd, void* buf, int bufsz, struct sockaddr_storage* client, int connected);
#endif
