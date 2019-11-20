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

//dynamic data allocation and deallocation
dyndata_t* allocate_dyn(meta_t* meta, void* elem);
int free_dyn(dyndata_t* elem);
int swap_dyn(dyndata_t* a, dyndata_t* b);

//miscellaneous
//int listToArray(t_algo_meta* dList, t_algo_meta* dArray, e_listtoarray action);
//int arrayToList(t_algo_meta* dArray, t_algo_meta* dList, e_listtoarray action);
//int arrayToAVL(t_algo_meta* dArray, t_algo_meta* dAVL, e_listtoarray action);
//
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
//void* insertAVL(t_algo_meta* meta, void* avl, void* toAdd);
//void display_AVL_tree(t_algo_meta* meta, void* avl, char dir, char* (*toString)(void*));
//void* rotate_AVL(t_algo_meta* meta, void* avl, e_rotation side);
//int get_AVL_balance(t_algo_meta* meta, void* avl);
//int foreachAVL(t_algo_meta* meta, void* avl, void* parameter, int (*doAction)(void*, void*));
//void* search_AVL(t_algo_meta* meta, void* avl, void* key);
//void* delete_AVL(t_algo_meta* meta, void* root, void* key);
//void* min_AVL_value(t_algo_meta* meta, void* avl);
//int delete_AVL_root(t_algo_meta* meta);
//
////File binary trees
//long index_tree(FILE* fp, long offset_start, long nb, t_algo_meta* meta);
//int searchall_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, t_algo_meta* list, int elem_size);
//int searchone_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, void* element, int elem_size);

#endif // ALGO_H_INCLUDED
