#include <mpi/mpi.h>
#include <mpich/mpi_proto.h>
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
    char str[5000000] = "Hello, world!\n";

    if(rank == 0)
    {
        int size = 1;
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
    }
    else // rank = 1
    {
        int size = 1;
        for(int i = 0; i < 5; ++i)
        {
            MPI_Recv(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(str, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

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