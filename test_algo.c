/*
** test_algo.c
** Contains all the tests concerning algorithmic features in libalgo.so
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 19/11/2019
*/
#include <time.h>
#include <stdlib.h>
#include "algo.h"
#include "dataset.h"

int setup_data(dataset_t** data, long nb);
int tst_bubblesortarray(void);
int tst_quicksortarray(void);
int tst_binarysearcharray(void);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    //tst_bubblesortarray();
    //tst_quicksortarray();
    tst_binarysearcharray();

	exit(EXIT_SUCCESS);
}

/************************************************************/
/*  I : Pointer to the array to allocate                    */
/*      Number of elements to allocate                      */
/*  P : Allocates an array and fills it with random data    */
/*  O :  0 -> Array created                                 */
/*      -1 -> Error                                         */
/************************************************************/
int setup_data(dataset_t** data, long nb)
{
    int r = 0;

    //allocate a space of nb times the size of dataset_t
    *data = calloc(nb, sizeof(dataset_t));
    if(!*data)
        return -1;

    for(int i=0 ; i<nb ; i++)
    {
        //generate a radom number between 1 and nb (inclusive)
        r = rand() % nb + 1;

        //fill in information relative to the random number
        (*data)[i].id = r;
        sprintf((*data)[i].type, "value : %d", r);
        (*data)[i].price = (float)r * 3.141593;
    }

    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the bubble sort algo with arrays          */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_bubblesortarray()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset_id, swap_dataset, copy_dataset};

    printf("/******************************************************************/\n");
    printf("/********************* tst_bubblesortarray ************************/\n");
    printf("/******************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error\n");
        return -1;
    }

    //display the unsorted data
    for(int i=0 ; i<20 ; i++)
        Print_dataset(arr.structure + (i*sizeof(dataset_t)), NULL);
    printf("----------------------------------------------------------\n");

    //sort it
    if(bubbleSortArray(&arr) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error\n");
        free(arr.structure);
        return -1;
    }

    //display all the datasets
    for(int i=0 ; i<20 ; i++)
        Print_dataset(arr.structure + (i*sizeof(dataset_t)), NULL);

    //free memory
    free(arr.structure);
    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the quick sort algo with arrays           */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_quicksortarray()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset_id, swap_dataset, copy_dataset};

    printf("/******************************************************************/\n");
    printf("/********************* tst_quicksortarray *************************/\n");
    printf("/******************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "quickSortArray : error\n");
        return -1;
    }

    //display the unsorted data
    for(int i=0 ; i<20 ; i++)
        Print_dataset(arr.structure + (i*sizeof(dataset_t)), NULL);
    printf("----------------------------------------------------------\n");

    //sort it
    if(quickSort(&arr, 0, arr.nbelements-1) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error\n");
        free(arr.structure);
        return -1;
    }

    //display all the datasets
    for(int i=0 ; i<20 ; i++)
        Print_dataset(arr.structure + (i*sizeof(dataset_t)), NULL);

    //free memory
    free(arr.structure);
    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the binary search algo with arrays        */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_binarysearcharray()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset_id, swap_dataset, copy_dataset};
    int x = 2, found=0;

    printf("/*********************************************************************/\n");
    printf("/********************* tst_binarysearcharray *************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "quickSortArray : error\n");
        return -1;
    }

    //sort it
    if(quickSort(&arr, 0, arr.nbelements-1) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error\n");
        free(arr.structure);
        return -1;
    }

    //display the sorted data
    for(int i=0 ; i<20 ; i++)
        Print_dataset(arr.structure + (i*sizeof(dataset_t)), NULL);
    printf("----------------------------------------------------------\n");

    //change comparison method
    arr.doCompare = compare_dataset_int;

    //search for 2
    found = binarySearchArray(&arr, &x);
    if(found == -1)
        printf("%d was not found\n", x);
    else
        printf("%d was found at index %d\n", x, found);

    //search for first occurence of 2
    found = binarySearchArrayFirst(&arr, &x);
    if(found == -1)
        printf("First occurence of %d was not found\n", x);
    else
        printf("First occurence of %d was found at index %d\n", x, found);

    //free memory
    free(arr.structure);
    return 0;
}
