#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

const int NumOfThreads = 8;

void * thread(void * ptr)
{
    int * id = static_cast<int *>(ptr);

    printf("Hello, World! from thread %d\n", *id);
    free(id);
    return NULL;
}

int main()
{

    pthread_t threads[NumOfThreads];

    for(int i = 0; i < NumOfThreads; ++i)
    {
        int * id = static_cast<int *>(malloc(sizeof(int)));
        *id = i;
        pthread_create(threads + i, NULL, thread, id);
    }


    for(int i = 0; i < NumOfThreads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}