#include <omp.h>
#include <stdio.h>

int main()
{
    #pragma omp parallel
    {
        int threadNum = omp_get_thread_num();
        printf("Hello, world! from thread %d\n", threadNum);
    }
}