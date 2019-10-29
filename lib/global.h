#ifndef GLOBAL_INDUS_H_INCLUDED
#define GLOBAL_INDUS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once
#define BACKLOG 10 // how many pending connections queue will hold

#endif // GLOBAL_INDUS_H_INCLUDED
