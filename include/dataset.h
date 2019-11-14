#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DATA_F  "l32sd"

typedef struct dataset_t dataset_t;

struct dataset_t{
    uint32_t    id;
    char        type[32];
    float       price;
    int32_t     height;
    dataset_t*  previous;
    dataset_t*  next;
};

// display methods
int Print_dataset(void* rec, void* nullable);
char* toString_dataset(void* current);

// dynamic structures methods
void* allocate_dataset(void);
int compare_dataset_id(void* a, void* b);
int copy_dataset(void* oldelem, void* newelem);
int swap_dataset(void* first, void* second);
void** dataset_right(void* current);
void** dataset_left(void* current);

#endif // DATASET_H_INCLUDED
