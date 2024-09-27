#include <stdlib.h> // you can remove this if your code doesn't use it
#include <string.h> // you can remove this if your code doesn't use it
// Define the enum to represent the sorted status
typedef enum {
    SORTED,
    SINGLE_INVERSION,
    NOT_SORTED
} SORTSTATUS;
typedef struct {
    SORTSTATUS status;
    size_t inversion_start;
    size_t inversion_end;
} StatusResult;
void msort_unopt(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray);
void insertion_sort(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
StatusResult check_sorted_status(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));


void msort(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
    const size_t INSERTION_SORT_THRESHOLD = 29; 
    StatusResult res;
    if (nmemb <= 1) return; 
    res= check_sorted_status(base, nmemb, size, compar); // linear time: O(2n)
    if(res.status==SORTED || res.status==SINGLE_INVERSION) return;
    else{
        if (nmemb <= INSERTION_SORT_THRESHOLD) { 
            insertion_sort(base, nmemb, size, compar);
            return;
        }
        else{
            void* tempArray; // Temporary array to hold merge result
            tempArray = malloc(nmemb * size); 
            msort_unopt(base,nmemb,size,compar,tempArray);
            free(tempArray);
        }
        
    }
}
void msort_unopt(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray){
    if (nmemb <= 1) return; // Base case
    char copiedFlag=0;
    char* leftArray,*rightArray,*leftPtr,*rightPtr,*out;
    size_t i=0,j=0,l=0;
    size_t mid = nmemb/2;
    
    msort_unopt(base, mid, size, compar,tempArray); // sort first half
    msort_unopt((char*)base + mid * size, nmemb - mid, size, compar,tempArray); // sort second half
    leftArray = (char*)base;
    rightArray = (char*)base + mid * size;

    // Pointers for iteration
    leftPtr = leftArray;
    rightPtr = rightArray;

    out = (char*)tempArray;
    for (size_t k = 0; k < nmemb; ++k) {
        if (leftPtr == rightArray) { 
            //memcpy(out, rightPtr, (nmemb - mid - j) * size);
            memcpy(base, tempArray, (mid+j) * size); 
            copiedFlag=1;
            break;
        }
        if (rightPtr == leftArray + nmemb * size) { 
            //memcpy(out, leftPtr, (mid - i) * size);
            memcpy((char*)base+l*size, leftPtr, (mid - i) * size); 
            memcpy(base, tempArray, (l) * size); 
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
    if(!copiedFlag) memcpy(base, tempArray, nmemb * size);  
}
void insertion_sort(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
    void* temp = malloc(size); 
    size_t i, j;
    void* prev_ptr;
    // Traverse the array starting from the second element
    for (i = 1; i < nmemb; i++) {
        memcpy(temp, (char*)base + i * size, size); 
        j = i;
        prev_ptr = (char*)base + (j - 1) * size;
        while (j > 0 && compar(prev_ptr, temp) > 0) {
            memcpy((char*)base + j * size, prev_ptr, size);
            j--;
            if (j > 0) {
                prev_ptr = (char*)base + (j - 1) * size; 
            }
        }
        memcpy((char*)base + j * size, temp, size); 
    }
    free(temp);   
}
StatusResult check_sorted_status(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
    StatusResult result;
    result.status = SORTED; 
    result.inversion_start = result.inversion_end = -1; 
    char* arr = (char*)base;
    int sing_inv_flag = 0;
    for (size_t i = 1; i < nmemb; ++i) {
        if (compar((void*)(arr + (i - 1) * size), (void*)(arr + i * size)) > 0) {
            sing_inv_flag = 1;
            result.inversion_start = i - 1;
            result.inversion_end = i;
            char* temp = malloc(size); 
            memcpy(temp, arr + result.inversion_start * size, size);
            memcpy(arr + result.inversion_start * size, arr + result.inversion_end * size, size);
            memcpy(arr + result.inversion_end * size, temp, size);
            free(temp);
            break;
        }
    }
    if (!sing_inv_flag) {
        return result; 
    }
    for (size_t i = 1; i < nmemb; ++i) {
        if (compar((void*)(arr + (i - 1) * size), (void*)(arr + i * size)) > 0) {
            // char* temp = malloc(size); 
            // memcpy(temp, arr + result.inversion_start * size, size);
            // memcpy(arr + result.inversion_start * size, arr + result.inversion_end * size, size);
            // memcpy(arr + result.inversion_end * size, temp, size);
            // free(temp);
            result.status = NOT_SORTED;
            return result;
        }
    }
    result.status = SINGLE_INVERSION;
    return result;
}
