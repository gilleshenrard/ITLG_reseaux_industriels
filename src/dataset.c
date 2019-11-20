/*
** dataset.c
** Library regrouping dataset-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 20/11/2019
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
int compare_dataset(void* a, void* b){
    dataset_t* tmp_a = (dataset_t*)a;
    dataset_t* tmp_b = (dataset_t*)b;

    if(tmp_a->id > tmp_b->id)
        return 1;
    else if(tmp_a->id < tmp_b->id)
        return -1;
    else
        return strcmp(tmp_a->type, tmp_b->type);
}

/****************************************************************************************/
/*  I : first dataset to compare                                                        */
/*      int to which compare the dataset                                                */
/*  P : Compares a dataset with the int received                                        */
/*  O :  1 if A > B                                                                     */
/*       0 if A = B                                                                     */
/*      -1 if A < B                                                                     */
/****************************************************************************************/
int compare_dataset_int(void* a, void* b){
    dataset_t* tmp_a = (dataset_t*)a;
    int* tmp_b = (int*)b;

    if(tmp_a->id > *tmp_b)
        return 1;
    else if(tmp_a->id < *tmp_b)
        return -1;
    else
        return 0;
}
