#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/time.h>

void executer(int rank)
{
    if(rank > 1)
        return;
    
    int N = 6;
    char str[20000] = "Hello, world!\n";

    if(rank == 0)
    {
        int size = 1;

        printf("---------------------Testing Send--------------------\n");
        for(int i = 0; i < 5; ++i)
        {
            printf("Testing for message size: %d\n", size);
            struct timeval t1, t2, t3, t4;
            printf("Executer %d preparing to send message\n", rank);
            gettimeofday(&t1, NULL);
            MPI_Send(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            printf("Executer %d has sent message, going to sleep\n", rank);
            sleep(2);
            printf("Exectuter %d is preparing to recieve message\n", rank);
            gettimeofday(&t3, NULL);
            MPI_Recv(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t4, NULL);

            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);
            int delta2 = (t4.tv_sec - t3.tv_sec) * 1000 + (t4.tv_usec - t3.tv_usec);

            printf("Send time: %d ms, recieve time: %d ms\n", delta1, delta2);

            size *= 10;
        }

        size = 1;
        printf("---------------------Testing Ssend--------------------\n");

        for(int i = 0; i < 5; ++i)
        {
            printf("Testing for message size: %d\n", size);
            struct timeval t1, t2, t3, t4;
            printf("Executer %d preparing to Ssend message\n", rank);
            gettimeofday(&t1, NULL);
            MPI_Ssend(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            printf("Executer %d has sent message, going to sleep\n", rank);
            sleep(2);
            printf("Exectuter %d is preparing to recieve message\n", rank);
            gettimeofday(&t3, NULL);
            MPI_Recv(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t4, NULL);

            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);
            int delta2 = (t4.tv_sec - t3.tv_sec) * 1000 + (t4.tv_usec - t3.tv_usec);

            printf("Send time: %d ms, recieve time: %d ms\n", delta1, delta2);

            size *= 10;
        }

        size = 1;
        printf("---------------------Testing Rsend--------------------\n");

        for(int i = 0; i < 5; ++i)
        {
            printf("Testing for message size: %d\n", size);
            struct timeval t1, t2, t3, t4;
            printf("Executer %d preparing to Rsend message\n", rank);
            gettimeofday(&t1, NULL);
            MPI_Rsend(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            printf("Executer %d has sent message, going to sleep\n", rank);
            sleep(2);
            printf("Exectuter %d is preparing to recieve message\n", rank);
            gettimeofday(&t3, NULL);
            MPI_Recv(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t4, NULL);

            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);
            int delta2 = (t4.tv_sec - t3.tv_sec) * 1000 + (t4.tv_usec - t3.tv_usec);

            printf("Send time: %d ms, recieve time: %d ms\n", delta1, delta2);

            size *= 10;
        }

        size = 1;
        printf("---------------------Testing Bsend--------------------\n");

        for(int i = 0; i < 5; ++i)
        {
            int buffer_size = (MPI_BSEND_OVERHEAD + size);
            // char* buffer = malloc(buffer_size);
            char buffer[10000 + MPI_BSEND_OVERHEAD];
            // Pass the buffer allocated to MPI so it uses it when we issue MPI_Bsend
            MPI_Buffer_attach(buffer, buffer_size);
            printf("Testing for message size: %d\n", size);
            struct timeval t1, t2, t3, t4;
            printf("Executer %d preparing to Bsend message\n", rank);
            gettimeofday(&t1, NULL);
            // printf("%s", str);
            MPI_Bsend(str, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            printf("Executer %d has sent message, going to sleep\n", rank);
            sleep(2);
            printf("Exectuter %d is preparing to recieve message\n", rank);
            gettimeofday(&t3, NULL);
            char str1[10000];
            MPI_Recv(str1, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t4, NULL);

            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);
            int delta2 = (t4.tv_sec - t3.tv_sec) * 1000 + (t4.tv_usec - t3.tv_usec);

            printf("Send time: %d ms, recieve time: %d ms\n", delta1, delta2);

            size *= 10;
            MPI_Buffer_detach(&buffer, &buffer_size);
            // free(buffer);
        }
    }
    else // rank = 1
    {
        int size = 0;

        
        size = 1;
        for(int i = 0; i < 5; ++i)
        {
            struct timeval t1, t2;
            MPI_Recv(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t1, NULL);
            MPI_Send(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);

            printf("Exectuter %d sent message for %d ms\n", rank, delta1);
            size *= 10;
        }

        size = 1;
        for(int i = 0; i < 5; ++i)
        {
            struct timeval t1, t2;
            MPI_Recv(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t1, NULL);
            MPI_Ssend(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);

            printf("Exectuter %d sent message for %d ms\n", rank, delta1);

            size *= 10;
        }

        size = 1;
        for(int i = 0; i < 5; ++i)
        {
             struct timeval t1, t2;
            MPI_Recv(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t1, NULL);
            MPI_Rsend(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);

            printf("Exectuter %d sent message for %d ms\n", rank, delta1);

            size *= 10;
        }

        size = 1;
        for(int i = 0; i < 5; ++i)
        {
            char buffer[10000 + MPI_BSEND_OVERHEAD];
            int buffer_size = (MPI_BSEND_OVERHEAD + size);
            struct timeval t1, t2;
            MPI_Recv(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            gettimeofday(&t1, NULL);
            MPI_Bsend(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
            gettimeofday(&t2, NULL);
            int delta1 = (t2.tv_sec - t1.tv_sec) * 1000 + (t2.tv_usec - t1.tv_usec);

            printf("Exectuter %d sent message for %d ms\n", rank, delta1);
            MPI_Buffer_detach(&buffer, &buffer_size);
            size *= 10;
        }

        
        
    }
}

int main(int argc, char ** argv)
{
    int commsize, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize); 
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    executer(my_rank);

    MPI_Finalize();
}