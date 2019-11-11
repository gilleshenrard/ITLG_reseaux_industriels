#ifndef ALGO_H_INCLUDED
#define ALGO_H_INCLUDED
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    void*   structure;
    long    nbelements;
    int     elementsize;
    int     (*doCompare)(void*, void*);     //comparison method
    int     (*doSwap)(void*, void*);        //swep method
    int     (*doCopy)(void*, void*);        //data copy method
    int     (*setSlot)(void*, void*);       //file slot assignment method
    int     (*getHeight)(void*);            //AVL height retrieving
    int     (*setHeight)(void*, int);       //AVL height setup
    void**  (*next)(void*);                 //retrieves either next node in a list, or the right child in AVL
    void**  (*previous)(void*);             //retrieves either previous node in a list, or the left child in AVL
} t_algo_meta;

int offset, offset_max;

#define LG_MAX 36

typedef enum {COPY, REPLACE} e_listtoarray;
typedef enum {RIGHT, LEFT} e_rotation;

//miscellaneous
int listToArray(t_algo_meta* dList, t_algo_meta* dArray, e_listtoarray action);
int arrayToList(t_algo_meta* dArray, t_algo_meta* dList, e_listtoarray action);
int arrayToAVL(t_algo_meta* dArray, t_algo_meta* dAVL, e_listtoarray action);

//Sorting algorithms
int bubbleSort(t_algo_meta*);
int bubbleSortList(t_algo_meta*);
int quickSortPartitioning(t_algo_meta*, long, long);
int quickSort(t_algo_meta*, long, long);

//Research algorithms
int binarySearch(t_algo_meta*, void*);
int binarySearchFirst(t_algo_meta*, void*);

//Dynamic lists
int insertListTop(t_algo_meta*, void*);
int popListTop(t_algo_meta*);
int insertListSorted(t_algo_meta*,  void*);
int foreachList(t_algo_meta*, void*, int (*doAction)(void*, void*));

//Arrays
int foreachArray(t_algo_meta*, void*, int (*doAction)(void*, void*));

//AVL trees
void* insertAVL(t_algo_meta* meta, void* avl, void* toAdd);
void display_AVL_tree(t_algo_meta* meta, void* avl, char dir, char* (*toString)(void*));
void* rotate_AVL(t_algo_meta* meta, void* avl, e_rotation side);
int get_AVL_balance(t_algo_meta* meta, void* avl);
int foreachAVL(t_algo_meta* meta, void* avl, void* parameter, int (*doAction)(void*, void*));
void* search_AVL(t_algo_meta* meta, void* avl, void* key);
void* delete_AVL(t_algo_meta* meta, void* root, void* key);
void* min_AVL_value(t_algo_meta* meta, void* avl);
int delete_AVL_root(t_algo_meta* meta);

//File binary trees
long index_tree(FILE* fp, long offset_start, long nb, t_algo_meta* meta);
int searchall_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, t_algo_meta* list, int elem_size);
int searchone_index(FILE* fp, long offset_root, void* key, t_algo_meta* index, void* element, int elem_size);

#endif // ALGO_H_INCLUDED
