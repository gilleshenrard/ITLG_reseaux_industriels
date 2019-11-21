#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct dyndata_t dyndata_t;

struct dyndata_t
{
    void*       data;
    int32_t     height;
    dyndata_t*  left;
    dyndata_t*  right;
};

typedef struct
{
    void*   structure;                      //pointer to the root of the whole structure
    long    nbelements;                     //number of elements in the structure
    int     elementsize;                    //size of a core element (actual data without algorithmic overlay)
    int     (*doCompare)(void*, void*);     //comparison method
} meta_t;


int offset, offset_max;

#define LG_MAX 36

typedef enum {COPY, REPLACE} e_listtoarray;
typedef enum {RIGHT, LEFT} e_rotation;

//miscellaneous
dyndata_t* allocate_dyn(meta_t* meta, void* elem);
int free_dyn(dyndata_t* elem);
int swap_dyn(dyndata_t* a, dyndata_t* b);
void* get_arrayelem(meta_t* meta, int i);

//Array, list and AVL transformation
int listToArray(meta_t* dList, meta_t* dArray, e_listtoarray action);
int arrayToList(meta_t* dArray, meta_t* dList, e_listtoarray action);
int arrayToAVL(meta_t* dArray, meta_t* dAVL, e_listtoarray action);

//Sorting algorithms
int bubbleSortArray(meta_t*);
int bubbleSortList(meta_t*);
int quickSortPartitioning(meta_t*, long, long);
int quickSortArray(meta_t*, long, long);

//Research algorithms
int binarySearchArray(meta_t*, void*);
int binarySearchArrayFirst(meta_t*, void*);

//Dynamic lists
int insertListTop(meta_t*, void*);
int popListTop(meta_t*);
int insertListSorted(meta_t*,  void*);
int freeDynList(meta_t* meta);
int foreachList(meta_t*, void*, int (*doAction)(void*, void*));

//Arrays
int foreachArray(meta_t*, void*, int (*doAction)(void*, void*));

////AVL trees
dyndata_t* insertAVL(meta_t* meta, dyndata_t* avl, void* toAdd);
void display_AVL_tree(meta_t* meta, dyndata_t* avl, char dir, char* (*toString)(void*));
dyndata_t* rotate_AVL(meta_t* meta, dyndata_t* avl, e_rotation side);
int get_AVL_balance(meta_t* meta, dyndata_t* avl);
int foreachAVL(meta_t* meta, dyndata_t* avl, void* parameter, int (*doAction)(void*, void*));
void* search_AVL(meta_t* meta, dyndata_t* avl, void* key);
dyndata_t* delete_AVL(meta_t* meta, dyndata_t* root, void* key);
dyndata_t* min_AVL_value(meta_t* meta, dyndata_t* avl);
int delete_AVL_root(meta_t* meta);

////File binary trees
//long index_tree(FILE* fp, long offset_start, long nb, t_algo_meta* meta);
//int searchall_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, t_algo_meta* list, int elem_size);
//int searchone_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, void* element, int elem_size);

#endif // ALGO_H_INCLUDED
