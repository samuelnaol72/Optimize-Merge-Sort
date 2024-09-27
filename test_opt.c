#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "msort_opt.h"
#define BATCHES 10         // Number of batches
#define TRIALS 1000          // Number of arrays per batch
#define LARGE_ARRAY_MIN 10000 // Minimum length for large arrays
#define SMALL_ARRAY_MAX 64    // Maximum length for small arrays
#define MAX_INPUT 1000000     // Maximum input size to allocate memory
void* tempArray;
/*-----------------------------------------------------------------------*/
static int 
IntIncreasingOrder(const void *A, const void *B)
{
    const int a = *(int *)A, b = *(int *)B;
    return (a > b) ? 1 : ((a < b) ? -1 : 0);
}

/*-----------------------------------------------------------------------*/
void msortHelper(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray);
void msort_UNOPT(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
    tempArray = malloc(nmemb * size); // Temporary array to hold merge result
    msortHelper(base,nmemb,size,compar,tempArray);
    free(tempArray);

}
void msortHelper(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray)
{
    if (nmemb <= 1) return; // Base case
    char copiedFlag=0;
    char* leftArray,*rightArray,*leftPtr,*rightPtr,*out;
    size_t i=0,j=0,l=0;
    size_t mid = nmemb/2;
    
    msortHelper(base, mid, size, compar,tempArray); // sort first half
    msortHelper((char*)base + mid * size, nmemb - mid, size, compar,tempArray); // sort second half
    leftArray = (char*)base;
    rightArray = (char*)base + mid * size;

    // Pointers for iteration
    leftPtr = leftArray;
    rightPtr = rightArray;

    out = (char*)tempArray;
    for (size_t k = 0; k < nmemb; ++k) {
        if (leftPtr == rightArray) { 
            //memcpy(out, rightPtr, (nmemb - mid - j) * size);
            memcpy(base, tempArray, (mid+j) * size); // The remaining righ half is at the right place,so just copy the temp array to base array
            copiedFlag=1;
            break;
        }
        if (rightPtr == leftArray + nmemb * size) { 
            //memcpy(out, leftPtr, (mid - i) * size);
            memcpy((char*)base+l*size, leftPtr, (mid - i) * size); //To copy remaining elements from the left Array to the base array
            memcpy(base, tempArray, (l) * size); // copying what is in tempArray to base array
            copiedFlag=1;
            break;
        }
        // Compare and merge elements
        int comp = compar((void*)leftPtr, (void*)rightPtr);
        if (comp <= 0) {
            memcpy(out, leftPtr, size);
            leftPtr += size;
            i++;
        } 
        else {
            memcpy(out, rightPtr, size);
            rightPtr += size;
            j++;
        }
        out += size;
        l++;
    }
    if(!copiedFlag) memcpy(base, tempArray, nmemb * size);  // Copy the merged result back to the original array
}
void insertion_sortt(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
    void* temp = malloc(size); // Temporary storage for the current element
    size_t i, j;
    void* prev_ptr;
    // Traverse the array starting from the second element
    for (i = 1; i < nmemb; i++) {
        memcpy(temp, (char*)base + i * size, size); // Copy the current element into temp storage
        j = i;
        prev_ptr = (char*)base + (j - 1) * size;
        
        // Shift elements to the right until the correct position for temp is found
        while (j > 0 && compar(prev_ptr, temp) > 0) {
            memcpy((char*)base + j * size, prev_ptr, size);
            j--;
            if (j > 0) {
                prev_ptr = (char*)base + (j - 1) * size; 
            }
        }
        memcpy((char*)base + j * size, temp, size); // Place temp in its correct position
    }
    free(temp);   
}
static int 
IntDecreasingOrder(const void *A, const void *B)
{
    const int a = *(int *)A, b = *(int *)B;
    return (a < b) ? 1 : ((a > b) ? -1 : 0);    
}

/* Function to make the array either fully sorted or with a single inversion */
void make_almost_sorted(int* A, int* B, int* C, int* D, size_t nmemb) {
    // Handle the case where the array is empty
    if (nmemb == 0) {
        return; // Nothing to do if there are no elements
    }

    // Generate a sorted array directly
    for (size_t i = 0; i < nmemb; ++i) {
        A[i] = B[i] = C[i] = D[i] = (int)i;
    }

    // Simulate a coin flip to decide between fully sorted or single inversion
    int coin_flip = rand() % 2; // Generate either 0 or 1 randomly

    if (coin_flip == 0 && nmemb > 1) {
        // Introduce a single inversion
        size_t inversion_index = rand() % (nmemb - 1); // Choose a random index to create the inversion
        
        // Swap elements at inversion_index and inversion_index + 1 in all arrays
        int temp = A[inversion_index];
        A[inversion_index] = A[inversion_index + 1];
        A[inversion_index + 1] = temp;

        B[inversion_index] = A[inversion_index];
        B[inversion_index + 1] = A[inversion_index + 1];

        C[inversion_index] = A[inversion_index];
        C[inversion_index + 1] = A[inversion_index + 1];

        D[inversion_index] = A[inversion_index];
        D[inversion_index + 1] = A[inversion_index + 1];
    }
    // If coin_flip == 1, the array remains fully sorted, so no additional changes are needed.
}

/*-----------------------------------------------------------------------*/
void generate_random_array(int* A, int* B, int* C, int* D, size_t nmemb) {
    for (size_t i = 0; i < nmemb; i++) {
        A[i] = B[i] = C[i] = D[i] = rand() % 1000000; // Random numbers between 0 and 999999
    }
}

/*-----------------------------------------------------------------------*/
int main(void)
{
    srand((unsigned int)time(NULL)); // Seed the random number generator
    int (*fun_ptr)(const void *, const void *);
    int *inputA, *inputB, *inputC, *inputD;
    size_t count;
    struct timespec begin, end;
    
    double total_msort_time = 0, total_msort_unopt_time = 0, total_insertion_time = 0;

    // Open CSV file for writing results
    FILE *fp = fopen("batch_results.csv", "w");
    // if (fp == NULL) {
    //     fprintf(stderr, "Failed to open file for writing\n");
    //     return -1;
    // }

    // Write CSV header
    fprintf(fp, "Batch Number,Optimized Merge Sort Time (s),Unoptimized Merge Sort Time (s),Insertion Sort Time (s),Faster Algorithm\n");

    inputA = malloc(sizeof(int) * MAX_INPUT);
    inputB = malloc(sizeof(int) * MAX_INPUT);
    inputC = malloc(sizeof(int) * MAX_INPUT);
    inputD = malloc(sizeof(int) * MAX_INPUT);


    // Loop over multiple batches
    for (int batch = 1; batch <= BATCHES; batch++) {
        double batch_msort_time = 0, batch_msort_unopt_time = 0, batch_insertion_time = 0;

        // 80% Small Arrays
        for (int trial = 0; trial < TRIALS * 0.8; trial++) {
            // Generate random size between 2 and 64
            count = (rand() % (64 - 2 + 1)) + 2;
            fun_ptr = (count % 2 == 0) ? IntIncreasingOrder : IntDecreasingOrder;
            // Make half of them almost sorted
            if (trial % 2 == 0) make_almost_sorted(inputA, inputB, inputC, inputD, count);
            else generate_random_array(inputA, inputB, inputC, inputD, count);
            // printf("input: ");
            // for (size_t j = 0; j < count; j++) {
            //     printf("%d ", inputA[j]);
            // // }
            // printf("\n");
            qsort(inputB, count, sizeof(int), fun_ptr);
            // Time optimized merge sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            msort(inputA, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);
            batch_msort_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;
            // Time unoptimized merge sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            msort_UNOPT(inputC, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);
            batch_msort_unopt_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;

            // Time insertion sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            insertion_sortt(inputD, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);

            // Validating
            for (size_t i = 0; i < count; i++) {
                if (inputA[i] != inputB[i] || inputC[i] != inputB[i] || inputD[i] != inputB[i]) {
                    printf("Test failed at index %zu\n", i);
                    printf("inputA: ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputA[j]);
                    }
                    printf("\n");

                    printf("inputB (reference, sorted using qsort): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputB[j]);
                    }
                    printf("\n");

                    printf("inputC (Unoptimized merge sort result): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputC[j]);
                    }
                    printf("\n");

                    printf("inputD (Insertion sort result): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputD[j]);
                    }
                    printf("\n");

                    return -1; // Exit early since there's a failure
                }
            }

            batch_insertion_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;   

        }
        // 20% large Arrays
        for (int trial = 0; trial < 10; trial++) {
            count = (rand() % (15000 - 10000 + 1)) + 10000;;
            fun_ptr = (count % 2 == 0) ? IntIncreasingOrder : IntDecreasingOrder;
            if (trial % 2 == 0) make_almost_sorted(inputA, inputB, inputC, inputD, count);
            else generate_random_array(inputA, inputB, inputC, inputD, count);
            qsort(inputB, count, sizeof(int), fun_ptr);
            // Time optimized merge sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            msort(inputA, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);
            batch_msort_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;
            // Time unoptimized merge sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            msort_UNOPT(inputC, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);
            batch_msort_unopt_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;

            // Time insertion sort
            clock_gettime(CLOCK_MONOTONIC, &begin);
            insertion_sortt(inputD, count, sizeof(int), fun_ptr);
            clock_gettime(CLOCK_MONOTONIC, &end);

            for (size_t i = 0; i < count; i++) {
                if (inputA[i] != inputB[i] || inputC[i] != inputB[i] || inputD[i] != inputB[i]) {
                    printf("Test failed at index %zu\n", i);

                    // Print the contents of the arrays
                    printf("Array contents:\n");
                    printf("inputA: ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputA[j]);
                    }
                    printf("\n");

                    printf("inputB (reference, sorted using qsort): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputB[j]);
                    }
                    printf("\n");

                    printf("inputC (Unoptimized merge sort result): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputC[j]);
                    }
                    printf("\n");

                    printf("inputD (Insertion sort result): ");
                    for (size_t j = 0; j < count; j++) {
                        printf("%d ", inputD[j]);
                    }
                    printf("\n");

                    return -1; // Exit early since there's a failure
                }
            }
            batch_insertion_time += (end.tv_sec - begin.tv_sec) + (end.tv_nsec - begin.tv_nsec) / 1e9;   
        }
        // Determine which algorithm is faster for this batch
        const char *faster_algorithm = (batch_msort_time < batch_msort_unopt_time && batch_msort_time < batch_insertion_time) ? 
                                       "Optimized Merge Sort" : 
                                       (batch_msort_unopt_time < batch_insertion_time ? "Unoptimized Merge Sort" : "Insertion Sort");

        // Write batch results to the CSV file
        fprintf(fp, "%d,%.9f,%.9f,%.9f,%s\n", batch, batch_msort_time, batch_msort_unopt_time, batch_insertion_time, faster_algorithm);
        // Accumulate the total time across all batches
        total_msort_time += batch_msort_time;
        total_msort_unopt_time += batch_msort_unopt_time;
        total_insertion_time += batch_insertion_time;
    }

    // Write overall results
    printf("Overall Results: \n");
    printf("Total Optimized Merge Sort Time: %.9f seconds\n", total_msort_time);
    printf("Total Unoptimized Merge Sort Time: %.9f seconds\n", total_msort_unopt_time);
    printf("Total Insertion Sort Time: %.9f seconds\n", total_insertion_time);

    // Clean up
    free(inputA); free(inputB); free(inputC); free(inputD); fclose(fp);

    printf("Results written to batch_results.csv\n");

    return 0;
}
