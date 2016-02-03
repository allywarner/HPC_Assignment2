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
#include <iterator>
#include <sstream>
#include <time.h>
#include <ctime>
#include <vector>
#include <cmath>

using namespace std;
void seqScan(void*,size_t,size_t);
double addDouble(const void*, const void*);

//three dimensional percision vector
struct threeDimVec {
    double x;
    double y;
    double z;
};

threeDimVec addThreeDimVec(const void*,const void*);

//parallel scan
void genericScan(void* arrayBase, size_t arraySize, size_t elementSize){
    
    int processes = 0;
    
#pragma omp parallel
    processes = omp_get_num_threads();
    
    if (arraySize <= processes) {
        seqScan(arrayBase,arraySize,elementSize);
        return;
    }
    
    char *newArray = new char[processes*elementSize];
    
//up sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        
        int start = (threadID * arraySize)/processes;
        int end = ((threadID+1)*arraySize)/processes;
        
        char* arrayBaseChar = (char*)arrayBase;
        
        seqScan(arrayBaseChar+start*elementSize,end-start,elementSize);
        
        memcpy(newArray+(threadID*elementSize),arrayBaseChar+(end-1)*elementSize,elementSize);
    }

//recursive call for scan
#pragma omp barrier
    genericScan(newArray,processes,elementSize);
    
//down sweep
#pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        
        if (threadID != 0){
            char* arrayBaseChar = (char*)arrayBase;
            
            int start = ((threadID)*arraySize)/processes;
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
    
    delete [] newArray;
}

//sequential scan
void seqScan(void* arrayBase, size_t arraySize,size_t elementSize){
    
    char* arrayBaseChar = (char*)arrayBase;
    
    for (int i = 1; i < arraySize; i++) {
        if(elementSize == sizeof(threeDimVec)){
            *((threeDimVec*)(arrayBaseChar + i*elementSize)) = addThreeDimVec(arrayBaseChar+i*elementSize,arrayBaseChar+(i-1)*elementSize);
        } else {
            *((double*)(arrayBaseChar+i*elementSize)) = addDouble(arrayBaseChar+i*elementSize,arrayBaseChar+(i-1)*elementSize);
        }
    }
}

//create random three dimensional vectors
threeDimVec randThreeDimVec(){
    double x = ((double)rand())/RAND_MAX;
    double y = ((double)rand())/RAND_MAX;
    double z = ((double)rand())/RAND_MAX;
    
    return {x,y,z};
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
    double d1 = *(double *)a;
    double d2 = *(double *)b;
    double addedDouble = d1 + d2;
    
    return addedDouble;
}

bool checkScanDouble(const vector<double>& vector1, const vector<double>& vector2)
{
    if (vector1.size() != vector2.size())
        return false;
    
    for (int i = 0; i < vector1.size(); ++i)
    {
        if (fabs(vector1[i] - vector2[i]) > 0.001)
            return false;
    }
    return true;
}

bool checkScanThreeDimVec(const vector<threeDimVec>& vector1, const vector<threeDimVec>& vector2)
{
    if (vector1.size() != vector2.size())
        return false;
    
    for (int i = 0; i < vector1.size(); ++i)
    {
        if (fabs(vector1[i].x - vector2[i].x) > 0.001)
            return false;
        if (fabs(vector1[i].y - vector2[i].y) > 0.001)
            return false;
        if (fabs(vector1[i].z - vector2[i].z) > 0.001)
            return false;
    }
    return true;
}

//MAIN FUNCTION
int main(int argc, char* argv[]){
    
    double time = 0;
    
    if (argc < 2) {
        cerr << "Error. Please input the length of an array to be scanned." << endl;
        return 1;
    }
    
    int arraySize = atoi(argv[1]);
    size_t elementSize;
    
    if (argc > 2) {
        
        elementSize = sizeof(threeDimVec);
        
        vector<threeDimVec> Array(arraySize);
        for (int i = 0; i < arraySize; i++) {
            Array[i] = randThreeDimVec();
        }
        
        vector<threeDimVec> ArrayForSeq = Array;
        
        clock_t startTime = clock();
        genericScan(&Array[0],arraySize,elementSize);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        seqScan(&ArrayForSeq[0],arraySize,elementSize);
        
        if (checkScanThreeDimVec(Array,ArrayForSeq))
        {
            cout << "3D Arrays are equal! Good job! :)" << endl;
            cout << "Time to complete: " << time << " seconds." << endl;
        }
        else
        {
            cout << "3D Arrays are not equal... :( Oh no!" << endl;
            cout << "Time to complete: " << time << " seconds." << endl;
        }
        
    } else {
        
        elementSize = sizeof(double);
        
        vector<double> Array(arraySize);
        for (int i = 0;i < arraySize; i++){
            Array[i] = ((double)rand())/RAND_MAX;
        }
        
        vector<double> ArrayForSeq = Array;
        
        clock_t startTime = clock();
        genericScan(&Array[0],arraySize,elementSize);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        seqScan(&ArrayForSeq[0],arraySize,elementSize);
        
        if (checkScanDouble(Array,ArrayForSeq))
        {
            cout << "1D Arrays are equal! Good job! :)" << endl;
            cout << "Time to complete: " << time << " seconds." << endl;
        }
        else
        {
            cout << "1D Arrays are not equal... :( Oh no!" << endl;
            cout << "Time to complete: " << time << " seconds." << endl;
        }
        
    }
}