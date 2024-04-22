#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

void executer(int rank, int num_of_executers, int N)
{
    double sum = 0;

    // printf("Exectuter %d: N=%d\n", rank, N);

    for(int i = rank + 1; i <= N; i+=num_of_executers)
        sum += 1. / i;

    if(rank != 0)
    {
        MPI_Send(&sum, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        // printf("Executer %d: sent sum=%lf to main\n", rank, sum);
    }
    else
    {
        for(int i = 1; i < num_of_executers; ++i)
        {
            double temp_sum = 0;
            MPI_Status status;
            MPI_Recv(&temp_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            // printf("Executer %d: recieved sum = %lf\n", rank, temp_sum);
            sum += temp_sum;
        }    

        printf("Executer %d: sum = %lf\n", rank, sum);
    }

}

int main(int argc, char ** argv)
{
    int commsize, my_rank;

    int N = atoi(argv[1]);

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize); 
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    executer(my_rank, commsize, N);

    MPI_Finalize();
}