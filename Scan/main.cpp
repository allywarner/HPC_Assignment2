//Ally Warner - u0680103
//Assignment 2
//High Performance Computing - CS 6230
//Due: February 3, 2015
//Parallel scan

#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

using namespace std;
void seqScan(void*,size_t,size_t);

//three dimensional percision vector
typedef struct _threeDimVec {
    double x;
    double y;
    double z;
} threeDimVec;

threeDimVec addThreeDimVec(const void*,const void*);
double addDouble(const void*, const void*);

//parallel scan
void genericScan(void* arrayBase, size_t arraySize, size_t elementSize){
    
    int processes = omp_get_num_threads();
    if (arraySize <= processes) {
        seqScan(arrayBase,arraySize,elementSize);
    }
    char *newArray = new char[processes*elementSize];
    
    //up sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        int start = (threadID * arraySize)/(processes-1);
        int end = ((threadID+1)*arraySize)/processes;
        
        char* arrayBaseChar = (char*)arrayBase;
        
        seqScan(arrayBaseChar + start*elementSize, end,elementSize);
        memcpy(newArray + threadID,arrayBaseChar+end, elementSize);
        genericScan(newArray,threadID,elementSize);
    }
    
    //down sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        char* arrayBaseChar = (char*)arrayBase;
        //not sure if this is right... don't want to double add
        int start = (threadID * arraySize)/(processes-1);
        int end = ((threadID+1)*arraySize)/processes;
        for (int i = start; i < end; i++) {
            if(elementSize == sizeof(threeDimVec)){
                *((threeDimVec*)(arrayBaseChar + i*elementSize)) = addThreeDimVec(arrayBaseChar+i*elementSize,newArray+(threadID-1)*elementSize);
            } else {
                *((double*)(arrayBaseChar+i*elementSize)) = addDouble(arrayBaseChar+i*elementSize,newArray+(threadID-1)*elementSize);
            }
        }
    }
    
}

//sequential scan
void seqScan(void* arrayBase, size_t arraySize,size_t elementSize){
    char* arrayBaseChar = (char*)arrayBase;
    for (int i = 1; i < arraySize - 1; i++) {
        if(elementSize == sizeof(threeDimVec)){
            *((threeDimVec*)(arrayBaseChar + i*elementSize)) = addThreeDimVec(arrayBaseChar+i*elementSize,arrayBaseChar+(i-1)*elementSize);
        } else {
            *((double*)(arrayBaseChar+i*elementSie)) = addDouble(arrayBaseChar+i*elementSize,arrayBaseChar+(i-1)*elementSize);
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
    threeDimVec addedVec;
    addedVec.x = vec1.x + vec2.x;
    addedVec.y = vec1.y + vec2.y;
    addedVec.z = vec1.z + vec2.z;
    
    return addedVec;
}

//double one dimensional vector addition
double addDouble(const void* a, const void* b){
    double a = *(double *)a;
    double b = *(double *)b;
    double addedDouble = a + b;
    
    return addedDouble;
}

int main(int argc, char* argv[]){
    
}