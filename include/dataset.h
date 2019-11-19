#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DATA_F  "l31sd"

typedef struct dataset_t dataset_t;

struct dataset_t{
    uint32_t    id;
    char        type[32];
    float       price;
};

// display methods
int Print_dataset(void* rec, void* nullable);
char* toString_dataset(void* current);

// dynamic structures methods
int compare_dataset_id(void* a, void* b);
int copy_dataset(void* oldelem, void* newelem);
int swap_dataset(void* first, void* second);

#endif // DATASET_H_INCLUDED
