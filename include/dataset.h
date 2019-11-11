#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>

typedef struct dataset_t dataset_t;

struct dataset_t{
    int32_t     id;
    char        type[32];
    float       price;
    int32_t     height;
    dataset_t*  previous;
    dataset_t*  next;
};

// display methods
void Print_dataset(void* rec);
char* toString_dataset(void* current);

// dynamic structures methods
void* allocate_dataset(void);
int compare_dataset_index_int(void* a, void* b);
int copt_dataset(void* oldelem, void* newelem);
int swap_dataset(void* first, void* second);
void** dataset_right(void* current);
void** dataset_left(void* current);
void* free_dataset(void* dataset, void* nullable);

#endif // DATASET_H_INCLUDED
