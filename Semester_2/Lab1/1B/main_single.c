#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

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
    const int ISIZE = atoi(argv[1]);
    const int JSIZE = atoi(argv[2]);

    double** a = (double **) calloc(ISIZE, sizeof(double**));
    handleCallocError(a);
    for(int i = 0; i < ISIZE; ++i)
    {
        a[i] = (double*) calloc(JSIZE, sizeof(double*));
        handleCallocError(a[i]);
    }

    int i, j;
    FILE *ff;
    for (i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
        {
            a[i][j] = 10*i +j;
        }
    }

    double start = omp_get_wtime();

    for (i=0; i<ISIZE-3; i++)
    {
        for (j = 4; j < JSIZE;j++)
        {
            a[i][j] = sin(0.04*a[i+3][j-4]);
        }
    }

    double stop = omp_get_wtime();
    printf("Time spent: %lf sec\n", (stop - start));

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
}
