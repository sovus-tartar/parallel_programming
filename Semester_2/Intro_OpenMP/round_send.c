#include <omp.h>
#include <stdio.h>
#include <string.h>

int main()
{
    
    int N = omp_get_max_threads();
    char helloString[1024] = {"Begin\n"};

    #pragma omp parallel for ordered
    {
        for (int i = 1; i <= N; ++i)
        {
            #pragma omp ordered
            {
                char tempString[50];
                sprintf(tempString, "Hello from thread %d\n", omp_get_thread_num());
                printf("Thread %d is going to concatenate helloString with: %s", omp_get_thread_num(), tempString);
                
                strcat(helloString, tempString);
            }

        }
    }

    printf("\nResulted helloString:\n%s", helloString);

}