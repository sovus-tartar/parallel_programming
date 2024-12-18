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
    int i, j;
    FILE *ff;
    for (i=0; i<ISIZE; i++)
    {
        for (j=0; j<JSIZE; j++){
            a[i][j] = 10*i +j;
        }
    }
    for (i=0; i<ISIZE-4; i++)
    {
        for (j = 5; j < JSIZE; j++){
            a[i][j] = sin(0.1*a[i+4][j-5]);
        }
    }
    ff = fopen("result_single.txt","w");
    for(i=0; i < ISIZE; i++)
    {
        for (j=0; j < JSIZE; j++){
            fprintf(ff,"%f ",a[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);
}