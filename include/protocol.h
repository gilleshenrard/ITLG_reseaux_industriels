#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED
#include "algo.h"
#include "network.h"
#include "serialisation.h"

#define MAXDATASIZE 4096 // max number of bytes we can get at once
#define HEAD_F      "LLQ"

#define SLIST       0
#define SFILE       1
#define SSTRING     2

typedef struct{
    uint32_t nbelem;
    uint32_t stype;
    uint64_t szelem;
}head_t;

int prcv(int sockfd, void* structure, void (*doPrint)(char*, ...));
int psnd(int sockfd, void* structure, head_t* header, int (*doSendList)(void*,void*), void (*doPrint)(char*, ...));

#endif // PROTOCOL_H_INCLUDED
