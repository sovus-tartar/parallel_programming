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

    // требуется обеспечить измерение времени работы данного цикла
    double start = omp_get_wtime();

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            a[i][j] = sin(2*a[i][j]);
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