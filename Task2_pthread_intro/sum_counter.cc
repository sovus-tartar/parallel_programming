#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

const int NumOfThreads = 8;

struct Args
{
    int id;
    int startFrom;
    int endWith;
};


void * thread(void * ptr)
{
    Args * args = static_cast<Args *>(ptr);

    double * sum = static_cast<double *>(malloc(sizeof(int)));

    for(int i = args->startFrom; i < args->endWith; ++i)
        *sum += 1. / i;


    return sum;
}

int main(int argc, char ** argv)
{
    int N = atoi(argv[1]);

    pthread_t threads[NumOfThreads];

    int curr = 1;

    for(int i = 0; i < NumOfThreads; ++i)
    {
        Args * args = static_cast<Args *>(malloc(sizeof(Args)));
        args->id = i;
        args->startFrom = curr;
        
        if(i != (NumOfThreads - 1))
            curr += N / NumOfThreads;
        else
            curr = N + 1;

        args->endWith = curr;

        pthread_create(threads + i, NULL, thread, args);
    }

    double sum = 0;

    for(int i = 0; i < NumOfThreads; ++i)
    {
        double * temp;
        pthread_join(threads[i], reinterpret_cast<void**>(&temp));
        sum += *temp;
        
        free(temp);
    }

    printf("sum by Threads = %lf\n", sum);

    double realSum = 0;

    for(int i = 1; i <= N; ++i)
        realSum += 1. / i;

    printf("real Sum = %lf\n", realSum);

    return 0;
}