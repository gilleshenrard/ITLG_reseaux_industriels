#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DATA_F  "l31sd"

typedef struct{
    uint32_t    id;
    char        type[32];
    float       price;
}dataset_t;

// display methods
int Print_dataset(void* rec, void* nullable);
char* toString_dataset(void* current);

// dynamic structures methods
int compare_dataset(void* a, void* b);
int compare_dataset_int(void* a, void* b);

#endif // DATASET_H_INCLUDED
