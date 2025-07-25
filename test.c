#include <stdio.h>
#include <stdlib.h>
#include "msort.h"

#define MAX_INPUT 1000000

/*-----------------------------------------------------------------------*/
static int 
IntIncreasingOrder(const void *A, const void *B)
{
    const int a = *(int *)A, b = *(int *)B;
    return (a > b) ? 1 : ((a < b) ? -1 : 0);
}
/*-----------------------------------------------------------------------*/
static int 
IntDecreasingOrder(const void *A, const void *B)
{
    return -IntIncreasingOrder(A, B);    
}
/*-----------------------------------------------------------------------*/
int 
main(void)
{
    int *inputA, *inputB;
    int i = 0, j, count = 0;
    int increasing = 0;

    inputA = malloc (sizeof(int) * MAX_INPUT);
    inputB = malloc (sizeof(int) * MAX_INPUT);
    if (inputA == NULL|| inputB == NULL) {
        fprintf(stderr, "memory allocation failed\n");
        return -1;
    }

    // get the input
    while (count < MAX_INPUT) {
        if (scanf("%d", &j) != 1)
            break; 
        // printf("input %d\n", j);
        inputA[i] = inputB[i] = j;
        i++;
        count++;
    }
    if (count == 0) {
        fprintf(stderr, "no input inserted\n");
        return -1;
    }

    increasing = (count % 2 == 0);
    printf("Sorting %d numbers in %s order\n", 
            count, increasing ? "increasing" : "decreasing");
    
    // merge sort
    msort(inputA, count, sizeof(int), 
            increasing ? IntIncreasingOrder : IntDecreasingOrder);

    // quick sort
    qsort(inputB, count, sizeof(int), 
            increasing ? IntIncreasingOrder : IntDecreasingOrder);

    // comparison
    for (i = 0; i < count; i++){
        if(inputA[i] != inputB[i]){
            printf("Tests Failed\n");
            return -1;
        }

    }
    printf("All Tests Passed!\n");
    
    // clean up
    free(inputA);
    free(inputB);
    
    return 0;
}
