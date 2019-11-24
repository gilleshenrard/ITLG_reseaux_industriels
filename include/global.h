#ifndef GLOBAL_INDUS_H_INCLUDED
#define GLOBAL_INDUS_H_INCLUDED

#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXDATASIZE 4096 // max number of bytes we can get at once
#define TIMEOUT     5
#define HEAD_F      "lQ"

typedef struct{
    uint32_t nbelem;
    uint64_t szelem;
}head_t;

#endif // GLOBAL_INDUS_H_INCLUDED
