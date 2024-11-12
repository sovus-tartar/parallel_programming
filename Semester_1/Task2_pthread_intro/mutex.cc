#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

const int NumOfThreads = 16;

struct Args
{
    int id;
    int startFrom;
    int endWith;
};

char varStr[1024] = {};
pthread_mutex_t m1;

void * thread(void * ptr)
{
    Args * args = static_cast<Args *>(ptr);


    pthread_mutex_lock(&m1);

    char temp[100];
    sprintf(temp, "Hello from thread %d\n", args->id);
    strcat(varStr, temp);


    pthread_mutex_unlock(&m1);

    free(ptr);

    return NULL;
}

int main(int argc, char ** argv)
{
    // int N = atoi(argv[1]);

    pthread_t threads[NumOfThreads];

    pthread_mutex_init(&m1, NULL);
    // pthread_mutex_unlock(&m1);
    int curr = 1;

    for(int i = 0; i < NumOfThreads; ++i)
    {
        Args * args = static_cast<Args *>(malloc(sizeof(Args)));
        args->id = i;
        args->startFrom = curr;

        args->endWith = curr;

        pthread_create(threads + i, NULL, thread, args);
    }

    for(int i = 0; i < NumOfThreads; ++i)
    {
        double * temp;
        pthread_join(threads[i], NULL);
    }

    printf(varStr);

    return 0;
}