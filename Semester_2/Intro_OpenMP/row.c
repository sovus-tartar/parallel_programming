#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char ** argv)
{
    int N = atoi(argv[1]);
    double sum = 0;

    printf("Sum for N = %d\n", N);

    #pragma omp parallel for reduction (+:sum)
    for (int i = 1; i <= N; ++i)
    {
        sum += (1 / (double) i);
    }


    printf("Sum = %lf\n", sum);

    double sumVerified = 0;

    for (int i = 1; i <= N; ++i)
    {
        sumVerified += 1 / (double) i;
    }

    printf("sumVerified = %lf, diff = %lf\n", sumVerified, fabs(sumVerified - sum));
    return 0;
}