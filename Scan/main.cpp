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

//parallel scan
void genericScan(void* arrayBase, size_t arraySize, size_t elementSize, void (*oper)(void *x1, void *x2)){
    
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
        seqScan(arrayBase + start*elementSize, end);
        newArray[threadID] = arrayBase(end);
    }
    
    genericScan(newArray,threadID,elementSize);
    
//down sweep
#pragma omp parallel
    {
        for (int i = start; i < end; i++) {
            arrayBase[i] = arrayBase[i] + newArray[threadID -1];
        }
    }

}

//sequential scan
void seqScan(void* arrayBase, size_t arraySize,size_t elementSize){
    for (int i = 1; i < arraySize - 1; i++) {
        arrayBase[i] = arrayBase[i] + arrayBase[i-1];
    }
}

int main(int argc, char* argv[]){

}