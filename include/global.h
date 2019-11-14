#ifndef GLOBAL_INDUS_H_INCLUDED
#define GLOBAL_INDUS_H_INCLUDED

#include <stdlib.h>
#include <sys/wait.h>
#include <stdint.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once
#define TIMEOUT     5

typedef struct{
    uint32_t nbelem;
    uint32_t szelem;
}head_t;

#endif // GLOBAL_INDUS_H_INCLUDED
