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

//parallel scan
void genericScan(void* arrayBase, size_t arraySize, size_t elementSize, void (*oper)(void *x1, void *x2)){
    
    processes = omp_get_num_threads();
    if (arraySize <= processes) {
        seqScan(arrayBase,arraySize)
    }

//up sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        int *newArray = new int[processes];
        int start = (threadID * arraySize)/(processes-1);
        int end = ((threadID+1)*arraySize)/processes;
        seqScan(arrayBase + start, end);
        newArray[threadID] = arrayBase(end);
    }
    
    genericScan(newArray,threadID);
    
//down sweep
#pragma omp parallel
    {
        for (int i = start; i < end; i++) {
            arrayBase[i] = arrayBase[i] + newArray[threadID -1];
        }
    }

}

//sequential scan
void seqScan(void* arrayBase, size_t arraySize){
    for (int i = 1; i < arraySize - 1; i++) {
        arrayBase[i] = arrayBase[i] + arrayBase[i-1];
    }
}

int main(int argc, char* argv[]){

}