/*
** dataset.c
** Library regrouping dataset-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 19/11/2019
*/
#include "dataset.h"

/****************************************************************************************/
/*  I : dataset record to print                                                         */
/*  P : Prints an dataset record                                                        */
/*  O : /                                                                               */
/****************************************************************************************/
int Print_dataset(void* rec, void* nullable){
    dataset_t* ds = (dataset_t*)rec;

    printf("|%4d | %32s | %4.8f |\n",
            ds->id,
            ds->type,
            ds->price);

    return 0;
}

/************************************************************/
/*  I : record to summarise as a string                     */
/*      /                                                   */
/*  P : returns a string representing the dataset           */
/*  O : /                                                   */
/************************************************************/
char* toString_dataset(void* current){
    dataset_t* tmp = (dataset_t*)current;
    return tmp->type;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// DYNAMIC ALLOCATION METHODS /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/****************************************************************************************/
/*  I : first dataset to compare                                                        */
/*      second dataset to compare                                                       */
/*  P : Compares two datasets by their ID                                               */
/*  O :  1 if A > B                                                                     */
/*       0 if A = B                                                                     */
/*      -1 if A < B                                                                     */
/****************************************************************************************/
int compare_dataset_id(void* a, void* b){
    dataset_t* tmp_a = (dataset_t*)a;
    dataset_t* tmp_b = (dataset_t*)b;

    if(tmp_a->id > tmp_b->id)
        return 1;
    else if(tmp_a->id < tmp_b->id)
        return -1;
    else
        return 0;
}

/****************************************************************************************/
/*  I : dataset to which copy data                                                      */
/*      dataset from which copy data                                                    */
/*  P : Copies all the fields of datasets from new to old                               */
/*  O :  0 if OK                                                                        */
/*      -1 otherwise                                                                    */
/****************************************************************************************/
int copy_dataset(void* oldelem, void* newelem){
    dataset_t* oldTuple = (dataset_t*)oldelem;
    dataset_t* newTuple = (dataset_t*)newelem;

    if(!oldelem || !newelem)
        return -1;

    //copy the data from the new dataset to the old one
    *oldTuple = *newTuple;

    return 0;
}

/************************************************************/
/*  I : datasets to swap                                    */
/*  P : Swaps two datasets                                  */
/*  O : 0 -> Swapped                                        */
/*     -1 -> Error                                          */
/************************************************************/
int swap_dataset(void* first, void* second){
    dataset_t tmp = {0};

    if(!first || !second)
        return -1;

    copy_dataset((void*)&tmp, first);
    copy_dataset(first, second);
    copy_dataset(second, (void*)&tmp);

    return 0;
}
