//Ally Warner - u0680103
//Assignment 2
//High Performance Computing - CS 6230
//Sorts arrays of type int, double, float, long and point which is a structure of two doubles using quicksort. Arrays can be any length.
//Added parallel functionality
//Due February 3, 2016

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctime>
#include <cstring>

using namespace std;
size_t partition(void*,size_t,size_t,int(*)(const void*, const void*));
void swap(void*,void*,size_t);

typedef struct _Point {
    double x;
    double y;
} Point;

//recursive quicksort algorithm
//Inputs: a pointer that points to the first element of the array, the size of the array, the datatype of the array and a compare function that will properly compare the datatype.
//acts on elements below and above the pivot recursively
void quickSort (void* arrayBase, size_t arraySize, size_t elementSize, int (*compar)(const void*,const void*)){
    
    if (arraySize > 0){
        
        size_t pivot = partition(arrayBase,arraySize,elementSize,compar);
        
        char* arrayBaseChar = (char*)arrayBase;
        
        quickSort(arrayBaseChar,pivot,elementSize,compar);
        quickSort(arrayBaseChar + (pivot+1)*elementSize,arraySize - (pivot+1),elementSize,compar);
        
    }
}

//partition algorithm - guts of quicksort
//Picks a pivot which is the last element of the array, then compares each element in the array to the pivot and swaps when necessary. Then swaps the pivot with the arraybase.
//Inputs: a pointer that points to the first element of the array, the size of the array, the datatype of the array and a compare function that will properly compare the datatype. (same as quicksort)
size_t partition(void* arrayBase,size_t arraySize, size_t elementSize, int(*compar)(const void*,const void*)){
    
    char* arrayBaseChar = (char*)arrayBase;
    
    char* pivot = arrayBaseChar + ((arraySize - 1)*elementSize);
    size_t *index = new size_t[arraySize];
    
    //first loop does indexing, second loop does swapping
#pragma omp parallel for
    for(int j = 0; j < arraySize-1;j++){
        if(compar(arrayBaseChar+(j*elementSize),pivot) <= 0){
            index[j] = 1;
       } else
           index[j] = 0;
    }
    
    //seq scan
    for (int i = 1; i < arraySize; i++) {
        index[i] = index[i] + index[i-1];
    }

#pragma omp parallel for
    for(int j = 0; j < arraySize-1;j++){
        if(compar(arrayBaseChar+(j*elementSize),pivot) <= 0){
            swap(arrayBaseChar+(j*elementSize),arrayBaseChar+(index[j]*elementSize),elementSize);
        }
    }
    
    swap(arrayBaseChar+(index[arraySize-1]*elementSize),pivot,elementSize);
    return index[arraySize-1];
}

//swap algorithm
//Inputs: two items you want to swap and the size of their datatype.
void swap(void* a, void* b, size_t size){
    // C99, use malloc otherwise
    // char serves as the type for "generic" byte arrays
    //Swap function found at: http://stackoverflow.com/questions/2232706/swapping-objects-using-pointers
    char temp[size];
    memcpy(temp,b,size);
    memcpy(b,a,size);
    memcpy(a,temp,size);
}

//Compare algorithms
//Inputs: two items you want to compare
//Outputs: -1 if a<b, 0 if a=b and 1 if a>b

//integers
int compareInt(const void* a, const void* b){
    //compare function found at:http://www.cplusplus.com/reference/cstdlib/qsort/
    int da = *(int*)a;
    int db = *(int*)b;
    if (da < db) return -1;
    if (db < da) return 1;
    return 0;
}

//doubles
int compareDouble(const void* a, const void* b){
    //compare function found at:http://www.cplusplus.com/reference/cstdlib/qsort/
    double da = *(double*)a;
    double db = *(double*)b;
    if (da < db) return -1;
    if (db < da) return 1;
    return 0;
}

//longs
int compareLong(const void* a, const void* b){
    //compare function found at:http://www.cplusplus.com/reference/cstdlib/qsort/
    long da = *(long*)a;
    long db = *(long*)b;
    if (da < db) return -1;
    if (db < da) return 1;
    return 0;
}

//floats
int compareFloat(const void* a, const void* b){
    //compare function found at:http://www.cplusplus.com/reference/cstdlib/qsort/
    float da = *(float*)a;
    float db = *(float*)b;
    if (da < db) return -1;
    if (db < da) return 1;
    return 0;
}

//points
int comparePoint(const void* a, const void *b) {
    Point da = *(Point *)a;
    Point db = *(Point *)b;
    if (da.y < db.y) return -1;
    if (db.y < da.y) return 1;
    if (da.y == db.y){
        if (da.x < db.x) return -1;
        if (db.x < da.x) return 1;
    }
    return 0;
}

//Checks if the array is sorted
//inputs: a pointer that points to the first element of the array, the size of the array, the datatype of the array and a compare function that will properly compare the datatype. (same as quicksort)
//outputs: 0 if not sorted, 1 if sorted
int checkSort(const void* array, size_t arrayLength, size_t elementSize, int (*compar)(const void*, const void*)){
    
    char* arrayChar = (char*)array;
    
    for (int j = 0; j < arrayLength-1; j++) {
        if (compar(arrayChar+(j*elementSize),arrayChar+((j+1)*elementSize)) == 1){
            return 0;
        }
    }
    return 1;
}

//creates random points
//Inputs: none
//Outputs: random points in the form (x,y)
Point randPoint(){
    double x = ((double)rand())/RAND_MAX;
    double y = ((double)rand())/RAND_MAX;
    return {x,y};
}

//MAIN FUNCTION
//Inputs: length of array as an int, data type of array in the type "int", "double", "float", "long" and "point", "run" or "test" are optional inputs.
int main(int argc, char* argv[]){
    
    int sortedFlag = 2;
    
    double time = 0;
    
    //error message if there are less than two inputs
    if (argc < 3) {
        cerr << "Error. Please input the length of the array and the datatype. :)" << endl;
        return 1;
    }
    //set a flag if "run" is input
    int flag = 0;
    if (argc == 4){
        flag = 1;
    }
    
    //generate a random array of a given length
    int arrayLength = atoi(argv[1]);
    string arrayType = argv[2];
    
    //for integers
    if(arrayType.compare("int")==0){
        int *intArray = new int[arrayLength];
        for (int i = 0;i < arrayLength; i++){
            intArray[i] = rand();
        }
        //timing quicksort algo
        clock_t startTime = clock();
        quickSort(intArray,arrayLength,sizeof(int),compareInt);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        //want to check sortedness before deleting
        if (flag == 1) {
            sortedFlag = checkSort(intArray,arrayLength,sizeof(int),compareInt);
        }
        
        delete [] intArray;
    }
    
    //for doubles
    else if(arrayType.compare("double") == 0){
        double *doubleArray = new double[arrayLength];
        for (int i = 0;i < arrayLength; i++){
          doubleArray[i] = ((double)rand())/RAND_MAX;
        }
        //timing quicksort algo
        clock_t startTime = clock();
        quickSort(doubleArray,arrayLength,sizeof(double),compareDouble);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        //want to check sortedness before deleting
        if (flag == 1) {
            sortedFlag = checkSort(doubleArray,arrayLength,sizeof(double),compareDouble);
        }
        
        delete [] doubleArray;
    }
    
    //for floats
    else if(arrayType.compare("float") == 0){
        float *floatArray = new float[arrayLength];
        for (int i = 0;i < arrayLength; i++){
            floatArray[i] = ((float)rand())/RAND_MAX;
        }
        //timing quicksort algo
        clock_t startTime = clock();
        quickSort(floatArray,arrayLength,sizeof(float),compareFloat);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        //want to check sortedness before deleting
        if (flag == 1) {
            sortedFlag = checkSort(floatArray,arrayLength,sizeof(float),compareFloat);
        }
        
        delete [] floatArray;
    }
    
    //for longs
    else if(arrayType.compare("long") == 0){
        long *longArray = new long[arrayLength];
        for (int i = 0;i < arrayLength; i++){
            longArray[i] = rand()*1200;
        }
        //timing quicksort algo
        clock_t startTime = clock();
        quickSort(longArray,arrayLength,sizeof(long),compareLong);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        //want to check sortedness before deleting
        if (flag == 1) {
            sortedFlag = checkSort(longArray,arrayLength,sizeof(long),compareLong);
        }
        
        delete [] longArray;
    }
    
    //for coordinate points structure
    else if (arrayType.compare("point") == 0){
        Point *pointArray = new Point[arrayLength];
        for (int i = 0; i < arrayLength; i++) {
            pointArray[i] = randPoint();
        }
        //timing quicksort algo
        clock_t startTime = clock();
        quickSort(pointArray,arrayLength,sizeof(Point),comparePoint);
        clock_t endTime = clock();
        time = double(endTime - startTime)/(CLOCKS_PER_SEC);
        
        //want to check sortedness before deleting
        if (flag == 1) {
            sortedFlag = checkSort(pointArray,arrayLength,sizeof(Point),comparePoint);
        }
        
        delete [] pointArray;
    }
    
    else {
        cout << "Error. Please input a correct datatype." << endl;
    }
    
    //if "test" or "run" are inputs
    //Test outputs how long the quicksort algo took to complete
    //Run outputs if the array is correctly sorted or not.
    if (argc == 4) {
        string thirdParameter = argv[3];
        if (thirdParameter.compare("test") == 0) {
            cout << "It took " << time << " seconds to sort " << arrayLength << " items in an array with datatype " << arrayType << "!" << endl;
        }
        else if (thirdParameter.compare("run") == 0){
            if (sortedFlag == 0) {
                cout << "This array is not sorted. :(" << endl;
            }
            else if (sortedFlag == 1){
                cout << "This array is sorted! :)" << endl;
            }
        }
    }
    
}