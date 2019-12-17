#ifndef DATASET_TEST_H_INCLUDED
#define DATASET_TEST_H_INCLUDED
#include <string.h>
#include <stdio.h>

typedef struct
{
    int     id;
    char    type[32];
    float   price;
}dataset_t;

int compare_dataset(void* a, void* b);
int compare_dataset_int(void* a, void* b);
int Print_dataset(void* elem, void* nullable);
char* toString_dataset(void* elem);

#endif // DATASET_TEST_H_INCLUDED
