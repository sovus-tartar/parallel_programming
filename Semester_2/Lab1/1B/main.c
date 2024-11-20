#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

void handleCallocError(void * ptr)
{
    if (ptr == NULL)
    {
        printf("Calloc failed to allocate memory, aborting...\n");
        abort();
    }
}

double f(double x)
{
    return sin(0.04 * x);
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

    FILE *ff;

    //подготовительная часть – заполнение некими данными
    for (int i = 0; i < x; i++)
    {
        for (int j=0; j < y; j++)
        {
            a[i][j] = 10*i +j;
        }
    }

    const uint8_t kNumberOfIterationsInBlock = 3;
    const uint8_t kNumberOfParallelIterations = ((x - 3) / kNumberOfIterationsInBlock) + 1;

    // требуется обеспечить измерение времени работы данного цикла
    double start = omp_get_wtime();


    for (int k = 0; k < kNumberOfParallelIterations; ++k)
    {
        #pragma omp parallel for
        for (int i = k * kNumberOfIterationsInBlock; i < (k + 1) * kNumberOfIterationsInBlock; ++i)
        {
            for (int j = 0; j < y; ++j)
            {
                a[i][j] = f(a[i + 3][j - 4])
            }
        }
    }


    double stop = omp_get_wtime();

#ifndef DISABLE_OUTPUT

    ff = fopen("result.txt","w");
    for(int i=0; i < x; i++)
    {
        for (int j=0; j < y; j++)
        {
            fprintf(ff,"%f ",a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);

#endif

    printf("Time spent: %lf sec\n", (stop - start));

    for(int i = 0; i < x; ++i)
    {
        free(a[i]);
    }
    free(a);
}