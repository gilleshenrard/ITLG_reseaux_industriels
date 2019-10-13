# Reseaux Industriels
## Exercice 1
---
### 1. Intro
This assignment aims to learn the basics of sockets programmation in C on UNIX-based systems by creating basic Daytime servers and clients.
Two executables are created (client and server) and both can be configured to handle either TCP or UDP.

Use :
```shell
./server port tcp|udp
./client host port tcp|udp
```

### 2. Set up
The assignment is programmed in C98, compiled with GNU GCC.
The code has been tested on :
* client : Ubuntu 18.04 LTS Desktop version, directly on the host
* Server : Ubuntu 18.04 LTS Server version, in a Virtualbox VM
A host-only connection has been created to allow both machines to communicate.

### 3. Current features
* Network-related functions :
```C
void *get_in_addr(struct sockaddr *sa);
int negociate_socket(struct addrinfo* sockinfo, int* sockfd, char ACTION);
int socket_to_ip(int* fd, char* address, int address_len);
```

### 4. Currently implemented in the final assignment
* Server, both in TCP and UDP
* Client, both in TCP and UDP (will work with a connected datagram socket in UDP)

### 5. To Do
* unit tests
* improve signals handling

### 6. Known issues
n/a
