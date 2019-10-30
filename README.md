# Reseaux Industriels
## Exercice 2 - Echo server
---
### 1. Intro
This assignment aims to learn the basics of sockets programmation in C on UNIX-based systems by creating basic Echo servers and clients (RFC862).
Two executables are created (client and server) and both can be configured to handle either TCP or UDP.

Use :
```shell
./server port tcp|udp
./client host port tcp|udp [message]
```

### 2. Set up
The assignment is programmed in C98, compiled with GNU GCC.
The code has been tested on :
* client : Ubuntu 18.04 LTS Desktop version, directly on the host
* Server : Ubuntu 18.04 LTS Server version, in a Virtualbox VM

A host-only connection (192.168.56.0/24) has been created to allow machines to communicate.
The IP addresses are set as followed :
* host : 192.168.56.1
* clapton server (TCP) : 192.168.56.10
* vaughan server (UDP) : 192.168.56.11

The code is edited and compiled in a directory shared with both guests, so it is centralised and only one version is edited and executed.

### 3. Current features
* Network-related functions :
```C
void *get_in_addr(struct sockaddr *sa);
int negociate_socket(char* host, char* service, int protocol, char ACTION, void (*on_error)(char*, ...));
int socket_to_ip(int* fd, char* address, int address_len);
```

* Screen-related functions :
```C
void setcolour(int foreground, int style);
void resetcolour();
void print_success(char* msg, ...);
void print_error(char* msg, ...);
```

A bash script (tests.sh) has been made to execute and test possible errors

### 4. Currently implemented in the final assignment
* Server, both in TCP and UDP
* Client, both in TCP and UDP (will work with a connected datagram socket in UDP)

### 5. To Do
* improve signals handling

### 6. Known issues
n/a
