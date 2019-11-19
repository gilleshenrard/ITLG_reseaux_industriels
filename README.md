# Reseaux Industriels
## Exercice 3 - Custom Protocol
---
### 1. Intro
This assignment aims to learn the basics of sockets programmation in C on UNIX-based systems by creating a custom data transmission protocol.
Two executables are created (client and server) and both will use TCP transmission.

Use :
```shell
./server port
./client host port
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
Note that the server hostname has to be added in /etc/hosts

The code is edited and compiled in a directory shared with the guest, so it is centralised and only one version is edited and executed.

### 3. Current features
* Network-related functions :
```C
void *get_in_addr(struct sockaddr *sa);
int negociate_socket(char* host, char* service, int socktype, char ACTION, void (*on_error)(char*, ...));
int socket_to_ip(int* fd, char* address, int address_len);
```

* Display-related functions :
```C
void format_output(char* final_msg, char* format, va_list* arg);
void print_success(char* msg, ...);
void print_error(char* msg, ...);
void print_neutral(char* msg, ...);
```

* Algorithmic-related functions :
```C
int insertListTop(t_algo_meta*, void*);
int popListTop(t_algo_meta*);
int insertListSorted(t_algo_meta*,  void*);
int foreachList(t_algo_meta*, void*, int (*doAction)(void*, void*));
```

A bash script [tests.sh](https://github.com/gilleshenrard/ITLG_reseaux_industriels/blob/master/tests.sh) has been made to execute and test possible errors

### 5. Protocol
At any connection via TCP on the port 3490, the server will send :
* A data header containing the metadata of the round of packages to be sent (serialised)

|  name  |  type    |             use               |
|:------:|:--------:|:-----------------------------:|
| nbelem | uint32_t | Number of packages to be sent |
| szelem | uint32_t | Size of each package          |

* The round of packages (serialised)

The client will then deserialise the packages and add the data to a linked list, then display it
For now, the packages consist of

|  name  |  type    |             use               |
|:------:|:--------:|:-----------------------------:|
|   id   | uint32_t | ID of the package             |
|  type  | char[32] | Message contained by the pkg  |
| price  |   float  | Float value (multiple of Pi)  |

### 6. Currently implemented in the final assignment
* Server
* Client
* Network-related functions
* Display-related functions
* Algorithmic-related functions
* Dataset-related functions
* Serialisation

### 6. To Do
* improve signals handling
* optimise algorithmics usage

### 7. Known issues
n/a
