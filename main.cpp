
// https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/index.html

#define POOLSIZE 32
#include <stddef.h>
#include "ComplexMemoryManager.h"


void NewDelArrayTest()
{
    Complex* array [1000];
    for (int i=0; i <5000; i++)
    {
        for (int j=0; j< 1000; j++)
        {
            array[j] = new Complex(i,j);
        }
        for(int j = 0; j< 1000; j++)
        {
            delete array[j];
        }
    }
    //time taken 0.210 s
    //time taken 0.172 s with memory manager
}

int main()
{

    NewDelArrayTest();

    return 0;
}


