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

int tst_bubblesortarray(void);
int setup_data(dataset_t** data, long nb);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    tst_bubblesortarray();

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
