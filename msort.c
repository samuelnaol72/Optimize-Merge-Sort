#include <stdlib.h>
#include <string.h>

void msortHelper(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray);

void msort(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)){
    void* tempArray= malloc(nmemb * size); // Temporary array to hold merge result
    // if (tempArray == NULL) {
    //     printf("Can't allocate memory for the tempArray array during merging");
    //     exit(EXIT_FAILURE);
    // }
    msortHelper(base,nmemb,size,compar,tempArray);
    free(tempArray);

}
void msortHelper(void* base, size_t nmemb, size_t size, int (*compar)(const void *, const void *),void* tempArray)
{
    if (nmemb <= 1) return; // Base case
    //char copiedFlag=0;
    char* leftArray,*rightArray,*leftPtr,*rightPtr,*out;
    //size_t i=0,j=0,l=0;
    size_t i=0,j=0;
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
            memcpy(out, rightPtr, (nmemb - mid - j) * size);
            //memcpy(base, tempArray, (mid+j) * size); // The remaining righ half is at the right place,so just copy the temp array to base array
            //copiedFlag=1;
            break;
        }
        if (rightPtr == leftArray + nmemb * size) { 
            memcpy(out, leftPtr, (mid - i) * size);
            //memcpy((char*)base+l*size, leftPtr, (mid - i) * size); //To copy remaining elements from the left Array to the base array
            //memcpy(base, tempArray, (l) * size); // copying what is in tempArray to base array
            //copiedFlag=1;
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
        //l++;
    }
    //if(!copiedFlag) memcpy(base, tempArray, nmemb * size);  // Copy the merged result back to the original array
    memcpy(base, tempArray, nmemb * size);
}




