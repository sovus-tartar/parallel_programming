#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

#define func(x) sin(0.1 *(x))

struct Borders
{
    int start;
    int end;
};

void handleCallocError(void* ptr)
{
    if (ptr == NULL)
    {
        printf("Calloc failed to allocate memory, aborting...\n");
        abort();
    }
}

int main(int argc, char **argv)
{
    omp_set_num_threads(atoi(argv[3]));
    printf("Number of threads: %d\n", atoi(argv[3]));

    const int kISize = atoi(argv[1]);
    const int kJSize = atoi(argv[2]);

    double** a = (double **) calloc(kISize, sizeof(double**));
    handleCallocError(a);
    for(int i = 0; i < kISize; ++i)
    {
        a[i] = (double*) calloc(kJSize, sizeof(double*));
        handleCallocError(a[i]);
    }

    FILE *ff;

    //подготовительная часть – заполнение некими данными
    for (int i = 0; i < kISize; i++)
    {
        for (int j = 0; j < kJSize; j++)
        {
            a[i][j] = 10 * i + j;
        }
    }
    
    const int kNumberOfIterationsInBlock = 4;
    const int kNumberOfParallelIterations = ((kISize - 4) / kNumberOfIterationsInBlock) + 1;

    // требуется обеспечить измерение времени работы данного цикла
    double start = omp_get_wtime();

    for (int k = 0; k < kNumberOfParallelIterations; ++k)
    {
        const int start = k * kNumberOfIterationsInBlock;
        const int stop = (((k + 1) * kNumberOfIterationsInBlock) < (kISize - 4)) ? ((k + 1) * kNumberOfIterationsInBlock) : kISize - 4;

        // printf("k = %d, i_start = %d, i_stop = %d\n", k, start, stop);

        #pragma omp parallel for
        for (int i = start; i < stop; ++i)
        {
            for (int j = 5; j < kJSize; ++j)
            {
                a[i][j] = func(a[i + 4][j - 5]);
            }
        }
    }

    double stop = omp_get_wtime();

    printf("Time spent: %lf sec\n", (stop - start));

#ifndef DISABLE_OUTPUT

    ff = fopen("result.txt","w");
    for(int i=0; i < kISize; i++)
    {
        for (int j=0; j < kJSize; j++)
        {
            fprintf(ff,"%f ", a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);

#endif

    for(int i = 0; i < kISize; ++i)
    {
        free(a[i]);
    }
    free(a);
}