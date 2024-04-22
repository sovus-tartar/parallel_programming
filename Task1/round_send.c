#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Num_of_cores must be less then 10

void executer(int rank, int num_of_executers)
{

    if(rank == 0)
    {
        char str[] = "Message went through 0";

        MPI_Send(str, strlen(str) + 1, MPI_CHAR, (rank + 1) % num_of_executers, 1, MPI_COMM_WORLD);
        printf("Executer %d: sent str \"%s\"\n", rank, str);
        char buffer[100];
        MPI_Status status;

        MPI_Recv(buffer, 100, MPI_CHAR, num_of_executers - 1, 1, MPI_COMM_WORLD, &status);
        printf("Executer %d: received str \"%s\"\n", rank, buffer);
        puts(buffer);

    }
    else if (rank < num_of_executers)
    {
        char buffer[100];
        MPI_Status status;
        MPI_Recv(buffer, 100, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
        printf("Executer %d: received str \"%s\"\n", rank, buffer);
        char str[2] = {rank + '0', 0};
        strcat(buffer, str);
        
        MPI_Send(buffer, strlen(buffer) + 1, MPI_CHAR, (rank + 1) % num_of_executers, 1, MPI_COMM_WORLD);
        printf("Executer %d: sent str \"%s\"\n", rank, buffer);
    }

}

int main(int argc, char ** argv)
{
    int commsize, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize); 
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    executer(my_rank, commsize);

    MPI_Finalize();
}