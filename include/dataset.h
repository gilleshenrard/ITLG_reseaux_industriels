#ifndef DATASET_H_INCLUDED
#define DATASET_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define FILENAMESZ  128
#define DATA_F  "128s"

// display methods
int Print_dataset(void* rec, void* nullable);
char* toString_dataset(void* current);

// dynamic structures methods
int compare_dataset(void* a, void* b);

#endif // DATASET_H_INCLUDED
