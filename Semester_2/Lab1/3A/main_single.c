#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    double** b = (double **) calloc(ISIZE, sizeof(double**));
    handleCallocError(b);
    for(int i = 0; i < ISIZE; ++i)
    {
        b[i] = (double*) calloc(JSIZE, sizeof(double*));
        handleCallocError(b[i]);
    }

    int i, j;
    FILE *ff;

    for (i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++)
        {
            a[i][j] = 10*i +j;
            b[i][j] = 0;
        }
    }
  
    ///начало измерения времени
    for (i=0; i<ISIZE; i++)
    {
        for (j = 0; j < JSIZE; j++)
        {
            a[i][j] = sin(0.1*a[i][j]);
        }
    }

    for (i=0; i<ISIZE-1; i++)
    {
        for (j = 0; j < JSIZE; j++)
        {
            b[i][j] = a[i+1][j]*1.5;
        }
    }
    
    ///окончание измерения времени

    ff = fopen("result_single.txt","w");
    for(i = 0; i < ISIZE; i++)
    {
        for (j = 0; j < JSIZE; j++)
        {
            fprintf(ff,"%f ", b[i][j]);
        }
        fprintf(ff,"\n");
    }

    for(int i = 0; i < ISIZE; ++i)
    {
        free(a[i]);
        free(b[i]);
    }
    free(a);
    free(b);
}
