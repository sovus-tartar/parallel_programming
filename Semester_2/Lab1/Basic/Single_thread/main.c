#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <sys/select.h>
#include <sys/time.h>

const unsigned long kNanoseconds = 1000000;

unsigned long getTimeOfDayDiff(struct timeval start, struct timeval stop)
{
    return (stop.tv_sec - start.tv_sec) * kNanoseconds + stop.tv_usec - start.tv_usec;
}

void handleCallocError(void * ptr)
{
    if (ptr == NULL)
    {
        printf("Calloc failed to allocate memory, aborting...\n");
        abort();
    }
}

int main(int argc, char **argv)
{
    const int ISIZE = atoi(argv[1]);
    const int JSIZE = atoi(argv[2]);

    double** a = (double **)calloc(ISIZE, sizeof(double**));
    handleCallocError(a);
    for(int i = 0; i < ISIZE; ++i)
    {
        a[i] = (double*)calloc(JSIZE, sizeof(double*));
        handleCallocError(a[i]);
    }

    int i = 0, j = 0;
    FILE *ff = NULL;

    struct timeval start = {}, stop = {};

    //подготовительная часть – заполнение некими данными
    for (i=0; i < ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
        {
            a[i][j] = 10*i +j;
        }
    }

    gettimeofday(&start, NULL);
    // требуется обеспечить измерение времени работы данного цикла
    for (i=0; i < ISIZE; i++)
    {
        for (j = 0; j < JSIZE; j++)
        {
            a[i][j] = sin(2*a[i][j]);
        }
    }
    gettimeofday(&stop, NULL);

#ifndef DISABLE_OUTPUT

    ff = fopen("result_single.txt","w");
    for(i=0; i < ISIZE; i++)
    {
        for (j=0; j < JSIZE; j++)
        {
            fprintf(ff,"%f ",a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);

#endif

    printf("Time spent: %lf sec\n", (double) getTimeOfDayDiff(start, stop) / kNanoseconds);

    for(int i = 0; i < ISIZE; ++i)
    {
        free(a[i]);
    }
    free(a);
}