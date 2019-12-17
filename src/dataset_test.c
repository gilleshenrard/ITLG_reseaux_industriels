#include "dataset_test.h"

int compare_dataset(void* a, void* b)
{
    dataset_t* A=(dataset_t*)a, *B=(dataset_t*)b;

    if(A->id > B->id)
        return 1;
    else if(A->id < B->id)
        return -1;
    else
        return strcmp(A->type, B->type);
}

int compare_dataset_int(void* a, void* b)
{
    dataset_t* A = (dataset_t*)a;
    int* B = (int*)b;

    if(A->id > *B)
        return 1;
    else if(A->id < *B)
        return -1;
    else
        return 0;
}

int Print_dataset(void* elem, void* nullable)
{
    dataset_t* A=(dataset_t*)elem;

    printf("%2d,%32s%4.8f\n", A->id, A->type, A->price);
    return 0;
}

char* toString_dataset(void* elem)
{
    dataset_t* A = (dataset_t*)elem;

    return A->type;
}
