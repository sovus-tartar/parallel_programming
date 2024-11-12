#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <sys/select.h>
#include <sys/time.h>

unsigned long getTimeOfDayDiff(struct timeval start, struct timeval stop)
{
    return (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
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
    const int x = atoi(argv[1]);
    const int y = atoi(argv[2]);

    double** a = (double **)calloc(x, sizeof(double**));
    handleCallocError(a);
    for(int i = 0; i < x; ++i)
    {
        a[i] = (double*)calloc(y, sizeof(double*));
        handleCallocError(a[i]);
    }
    int i = 0, j = 0;
    FILE *ff = NULL;

    struct timeval start = {}, stop = {};

    //подготовительная часть – заполнение некими данными
    for (i=0; i < x; i++)
    {
        for (j=0; j<y; j++)
        {
            a[i][j] = 10*i +j;
        }
    }

    gettimeofday(&start, NULL);
    // требуется обеспечить измерение времени работы данного цикла
    for (i=0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            a[i][j] = sin(2*a[i][j]);
        }
    }
    gettimeofday(&stop, NULL);

    ff = fopen("result.txt","w");
    for(i=0; i < x; i++)
    {
        for (j=0; j < y; j++)
        {
            fprintf(ff,"%f ",a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);

    printf("Time spent: %lf sec\n", (double) getTimeOfDayDiff(start, stop) / 1000000);

    for(int i = 0; i < x; ++i)
    {
        free(a[i]);
    }
    free(a);
}