/*
** dataset.c
** Library regrouping dataset-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 22/11/2019
*/
#include "dataset.h"

/****************************************************************************************/
/*  I : dataset record to print                                                         */
/*  P : Prints an dataset record                                                        */
/*  O : /                                                                               */
/****************************************************************************************/
int Print_dataset(void* rec, void* nullable){
    char* ds = (char*)rec;

    printf("%s\n", ds);

    return 0;
}

/************************************************************/
/*  I : record to summarise as a string                     */
/*      /                                                   */
/*  P : returns a string representing the dataset           */
/*  O : /                                                   */
/************************************************************/
char* toString_dataset(void* current){
    char* tmp = (char*)current;
    return tmp;
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
    char* tmp_a = (char*)a;
    char* tmp_b = (char*)b;

    return strcmp(tmp_a, tmp_b);
}
