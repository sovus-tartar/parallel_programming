#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

int main(int argc, char **argv)
{
    const int x = atoi(argv[1]);
    const int y = atoi(argv[2]);

    double a[x][y];
    int i, j;
    FILE *ff;

    //подготовительная часть – заполнение некими данными
    for (i=0; i < x; i++)
    {
        for (j=0; j<y; j++)
        {
            a[i][j] = 10*i +j;
        }
    }

    // требуется обеспечить измерение времени работы данного цикла

    double start = omp_get_wtime();

    #pragma omp parallel
    for (i=0; i < x; i++)
    {
        for (j = 0; j < y; j++)
        {
            a[i][j] = sin(2*a[i][j]);
        }
    }

    double stop = omp_get_wtime();

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
}