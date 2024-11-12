#include <omp.h>
#include <stdio.h>

int main()
{
    #pragma omp parallel
    printf("Hello, world! from thread %d\n", omp_get_thread_num());
}