/*
** test_algo.c
** Contains all the tests concerning algorithmic features in libalgo.so
** -------------------------------------------
** Made by Gilles Henrard
** Last modified : 20/11/2019
*/
#include <time.h>
#include <stdlib.h>
#include "algo.h"
#include "dataset.h"

int setup_data(dataset_t** data, long nb);
int tst_bubblesortarray(void);
int tst_quicksortarray(void);
int tst_binarysearcharray(void);
int tst_inserttoplist(void);
int tst_insertlistsorted(void);
int tst_bubblesortlist(void);
int tst_structuresconversion(void);
int tst_insertavl(void);
int tst_removeavl(void);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    //tst_bubblesortarray();
    //tst_quicksortarray();
    //tst_binarysearcharray();
    //tst_inserttoplist();
    //tst_insertlistsorted();
    //tst_bubblesortlist();
    //tst_structuresconversion();
    //tst_insertavl();
    tst_removeavl();

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
    int r = 0, r2 = 0;

    //allocate a space of nb times the size of dataset_t
    *data = calloc(nb, sizeof(dataset_t));
    if(!*data)
        return -1;

    for(int i=0 ; i<nb ; i++)
    {
        //generate a radom number between 1 and nb (inclusive),
        //  and a second between 1 and 5
        r = rand() % nb + 1;
        r2 = rand() % 5 + 1;

        //fill in information relative to the random number
        (*data)[i].id = r;
        sprintf((*data)[i].type, "value : %02d", r*r2);
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
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};

    printf("/******************************************************************/\n");
    printf("/********************* tst_bubblesortarray ************************/\n");
    printf("/******************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error while allocating the data\n");
        return -1;
    }

    //display the unsorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    //sort it
    if(bubbleSortArray(&arr) == -1)
    {
        fprintf(stderr, "bubbleSortArray : error while sorting the data\n");
        free(arr.structure);
        return -1;
    }

    printf("Data sorted:\n");
    //display all the datasets
    foreachArray(&arr, NULL, Print_dataset);

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
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};

    printf("/******************************************************************/\n");
    printf("/********************* tst_quicksortarray *************************/\n");
    printf("/******************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "quickSortArray : error while allocating the data\n");
        return -1;
    }

    //display the unsorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    //sort it
    if(quickSortArray(&arr, 0, arr.nbelements-1) == -1)
    {
        fprintf(stderr, "quickSortArray : error while sorting the data\n");
        free(arr.structure);
        return -1;
    }

    printf("Data sorted:\n");
    //display all the datasets
    foreachArray(&arr, NULL, Print_dataset);

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
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    int x = 2, found=0;

    printf("/*********************************************************************/\n");
    printf("/********************* tst_binarysearcharray *************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "binarysearchArray : error while allocating the data\n");
        return -1;
    }

    //sort it
    if(quickSortArray(&arr, 0, arr.nbelements-1) == -1)
    {
        fprintf(stderr, "binarysearchArray : error while sorting the data\n");
        free(arr.structure);
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
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

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the insertion at the head of a list       */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_inserttoplist()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t lis = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/********************* tst_inserttoplist *****************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "insertTopList : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    for(int i=0 ; i<20 ; i++)
    {
        if(insertListTop(&lis, arr.structure + i*sizeof(dataset_t)) == -1)
        {
            fprintf(stderr, "insertTopList : error while inserting the data\n");
            free(arr.structure);
            freeDynList(&lis);
            return -1;
        }
    }

    //free memory used by the array
    free(arr.structure);

    //list the content of the list, each time popping one element at its head
    while(lis.structure)
    {
        foreachList(&lis, NULL, Print_dataset);
        printf("Nb of elements: %ld\n", lis.nbelements);
        printf("----------------------------------------------------------\n");
        printf("Data with list head purged:\n");
        popListTop(&lis);

    }

    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the insertion in a sorted list            */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_insertlistsorted()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t lis = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/********************* tst_insertlistsorted **************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "insertListSorted : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    if(arrayToList(&arr, &lis, REPLACE) == -1)
    {
        fprintf(stderr, "insertListSorted : error while inserting the data\n");
        free(arr.structure);
        freeDynList(&lis);
        return -1;
    }

    printf("Data sorted:\n");
    //display the content of the list, and delete it
    foreachList(&lis, NULL, Print_dataset);
    freeDynList(&lis);

    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the bubble sort algo on a list            */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_bubblesortlist()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t lis = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/********************* tst_bubblesortlist ****************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "insertTopList : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    for(int i=0 ; i<20 ; i++)
    {
        if(insertListTop(&lis, arr.structure + i*sizeof(dataset_t)) == -1)
        {
            fprintf(stderr, "insertTopList : error while inserting the data\n");
            free(arr.structure);
            freeDynList(&lis);
            return -1;
        }
    }

    //free memory used by the array
    free(arr.structure);

    //sort the list using the bubble sort and display it
    bubbleSortList(&lis);
    printf("Data sorted:\n");
    foreachList(&lis, NULL, Print_dataset);

    //free the memory used by the list
    freeDynList(&lis);
    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the conversion algorithms on structures   */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_structuresconversion()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t lis = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/********************* tst_structuresconversion **********************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "insertTopList : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    //convert the array to a list and display it
    if(arrayToList(&arr, &lis, REPLACE) == -1)
    {
        fprintf(stderr, "structureConversion : error while creating the list\n");
        free(arr.structure);
        freeDynList(&lis);
        return -1;
    }
    printf("List created:\n");
    foreachList(&lis, NULL, Print_dataset);
    printf("array pointer: %p, number of elments: %ld\n", arr.structure, arr.nbelements);
    printf("----------------------------------------------------------\n");

    //convert the array to a list and display it
    if(listToArray(&lis, &arr, REPLACE) == -1)
    {
        fprintf(stderr, "structureConversion : error while creating the array\n");
        free(arr.structure);
        freeDynList(&lis);
        return -1;
    }
    printf("Array created:\n");
    foreachArray(&arr, NULL, Print_dataset);
    printf("list pointer: %p, number of elments: %ld\n", lis.structure, lis.nbelements);

    free(arr.structure);

    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the insertion in an AVL tree              */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_insertavl()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t avl = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/*************************** tst_insertavl ***************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "insertTopList : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    for(int i=0 ; i<arr.nbelements ; i++)
    {
        if((avl.structure = insertAVL(&avl, avl.structure, get_arrayelem(&arr, i))) == NULL)
        {
            fprintf(stderr, "insertAVL : error while creating the AVL\n");
            free(arr.structure);
            return -1;
        }
    }
    display_AVL_tree(&avl, avl.structure, 'T', toString_dataset);

    free(arr.structure);

    return 0;
}

/************************************************************/
/*  I : /                                                   */
/*  P : Tests out the removal from an AVL tree              */
/*  O :  0 -> Success                                       */
/*      -1 -> Error                                         */
/************************************************************/
int tst_removeavl()
{
    meta_t arr = {NULL, 20, sizeof(dataset_t), compare_dataset};
    meta_t avl = {NULL, 0, sizeof(dataset_t), compare_dataset};

    printf("/*********************************************************************/\n");
    printf("/*************************** tst_removeavl ***************************/\n");
    printf("/*********************************************************************/\n");

    //generate 20 random datasets
    if(setup_data((dataset_t**)&arr.structure, 20) == -1)
    {
        fprintf(stderr, "removeAVL : error while allocating the data\n");
        return -1;
    }

    //display the sorted data
    foreachArray(&arr, NULL, Print_dataset);
    printf("----------------------------------------------------------\n");

    for(int i=0 ; i<arr.nbelements ; i++)
    {
        if((avl.structure = insertAVL(&avl, avl.structure, get_arrayelem(&arr, i))) == NULL)
        {
            fprintf(stderr, "removeAVL : error while creating the AVL\n");
            free(arr.structure);
            return -1;
        }
    }

    foreachAVL(&avl, avl.structure, NULL, Print_dataset);
    printf("Nb of elements: %ld\n", avl.nbelements);

    free(arr.structure);

    return 0;
}
