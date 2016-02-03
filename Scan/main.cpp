//Ally Warner - u0680103
//Assignment 2
//High Performance Computing - CS 6230
//Due: February 3, 2015
//Parallel scan

#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
void seqScan(void*,size_t,size_t);

//three dimensional percision vector
typedef struct _threeDimVec {
    double x;
    double y;
    double z;
} threeDimVec;

//parallel scan
void genericScan(void* arrayBase, size_t arraySize, size_t elementSize){
    
    int processes = omp_get_num_threads();
    if (arraySize <= processes) {
        seqScan(arrayBase,arraySize,elementSize);
    }
    
    //up sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        int *newArray = new int[processes];
        int start = (threadID * arraySize)/(processes-1);
        int end = ((threadID+1)*arraySize)/processes;
        
        char* arrayBaseChar = (char*)arrayBase;
        
        seqScan(arrayBaseChar + start*elementSize, end,elementSize);
        newArray[threadID] = arrayBase[end];
    }
    
    genericScan(newArray,threadID,elementSize);
    
    //down sweep
#pragma omp parallel
    {
        char* arrayBaseChar = (char*)arrayBase;
        int threadID = omp_get_thread_num();
        for (int i = start; i < end; i++) {
            if(elementSize == threeDimVec){
                arrayBase[i] = addThreeDimVec(arrayBase[i],NewArray[threadID-1]);
            } else {
                arrayBase[i] = arrayBase[i] + newArray[threadID-1];
            }
        }
    }
    
}

//sequential scan
void seqScan(void* arrayBase, size_t arraySize,size_t elementSize){
    char* arrayBaseChar = (char*)arrayBase;
    for (int i = 1; i < arraySize - 1; i++) {
        if(elementSize == threeDimVec){
            arrayBase[i] = addThreeDimVec(arrayBase[i],arrayBase[i-1]);
        } else {
            arrayBase[i] = arrayBase[i] + arrayBase[i-1];
        }
    }
}

//create random three dimensional vectors
threeDimVec randThreeDimVec(){
    double x = ((double)rand())/RAND_MAX;
    double y = ((double)rand())/RAND_MAX;
    double z = ((double)rand())/RAND_MAX;
}

//three dimensional vector addition
threeDimVec addThreeDimVec(const void* a, const void* b){
    threeDimVec vec1 = *(threeDimVec *)a;
    threeDimVec vec2 = *(threeDimVec *)b;
    threeDimVec *addedVec = new threeDimVec[3];
    addedVec[1] = vec1.x + vec2.x;
    addedVec[2] = vec1.y + vec2.y;
    addedVec[3] = vec1.z + vec2.z;
}

int main(int argc, char* argv[]){
    
}