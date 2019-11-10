#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>

typedef struct{
    int32_t amount;
    char    type[32];
    float   price;
}dataset_t;

#endif // DATASET_H_INCLUDED
