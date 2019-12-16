# Reseaux Industriels
## Dossier 1 - Pseudo-FTP + Custom Protocol
---
### 1. Intro
This assignment aims to learn the basics of sockets programmation in C on UNIX-based systems by creating basic FTP transmission based on custom data transmission protocol.
Two executables are created (client and server) and both will use TCP transmission.

Use :
```shell
./server port path
./client host port
```

### 2. Current features
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

* Linked lists functions :
```C
int insertListTop(meta_t*, void*);
int popListTop(meta_t*);
int insertListSorted(meta_t*,  void*);
int freeDynList(meta_t* meta);
int foreachList(meta_t*, void*, int (*doAction)(void*, void*));
```

* Protocol functions :
```C
int prcv(int sockfd, void* structure, void (*doPrint)(char*, ...));
int psnd(int sockfd, void* structure, head_t* header, int (*doSendList)(void*,void*), void (*doPrint)(char*, ...));
```

A bash script [tests.sh](https://github.com/gilleshenrard/ITLG_reseaux_industriels/blob/master/tests.sh) has been made to execute and test possible errors

### 3. Protocol
#### a. Header structure
A structure defining a transmission header is defined as such :

|  name  |  type    |             use               |
|:------:|:--------:|:-----------------------------:|
| nbelem | uint32_t | Number of packages to be sent |
| stype  | uint32_t | Type of the structure sent    |
| szelem | uint64_t | Size of each package          |

#### b. Transmission functioning
- The sender will prepare a header and send it to the receiver
- The data is then serialised by the sender and deserialised by the receiver
- The receiver then prepares a header with the amount of bytes received,
    and sends it to the sender for acknowledgement
- The sender then compares the amount to be received and the one actually
    received

#### c. Data structures currently implemented
Currently, the protocol is up and running for:
- Strings
- Binary files
- Linked lists

### 4. Currently implemented in the final assignment
* Server
* Client
* Network-related functions
* Display-related functions
* Algorithmic-related functions
* Dataset-related functions
* Serialisation
* Protocol-related functions

### 5. To Do
* improve signals handling

### 6. Known issues
* RPATH had to be forced in lib/build.mk because libserialisation.so wasn't found at runtime of bin/client
* On client's side, lists sent by the server don't always come up as they should (blank spaces as first elements, incomplete elements, ...)
