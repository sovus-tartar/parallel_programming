#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

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
    MPI_Init(&argc, &argv);

    int commsize = 0;
    int rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int ISIZE = atoi(argv[1]);
    const int JSIZE = atoi(argv[2]);

    double** a = (double **) calloc(ISIZE, sizeof(double**));
    handleCallocError(a);
    for(int i = 0; i < ISIZE; ++i)
    {
        a[i] = (double*) calloc(JSIZE, sizeof(double*));
        handleCallocError(a[i]);
    }

    const int diff_J = (JSIZE - 3) / commsize;
    int start_J = diff_J * rank + 3;
    int end_J = diff_J * (rank + 1) + 3;

    if (JSIZE % commsize) 
    {
        if (rank < JSIZE % commsize) 
        {
            start_J += rank;
            end_J += rank + 1;
        } 
        else
        {
            start_J += (JSIZE % commsize);
            end_J += (JSIZE % commsize);
        }
    }

    if (rank == commsize - 1)
    {
        end_J = JSIZE;
    }

    int i, j;
    FILE *ff;

    for (int i = 0; i < ISIZE; ++i)
    {
        for (int j = 0; j < JSIZE; ++j)
        {
            a[i][j] = 10 * i + j;
        }
    }
    
    int localArraySize = end_J - start_J;

    double* localArray = calloc(JSIZE, sizeof(double));
    handleCallocError(localArray);

    int* recvcnts = calloc(commsize, sizeof(int));
    handleCallocError(recvcnts);

    int* displs = calloc(commsize, sizeof(int));
    handleCallocError(displs);
    
    for (int k = 0; k < commsize; ++k) 
    {
        MPI_Gather(&localArraySize, 1, MPI_INT, &recvcnts[0], 1, MPI_INT, k,
                MPI_COMM_WORLD);
    }

    displs[0] = 3;
    for (int i = 1; i < commsize; ++i)
    {
        displs[i] = displs[i - 1] + recvcnts[i - 1];
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start;
    if (rank == 0)
    {
        start = MPI_Wtime();
    }
//--------------------------
    if (commsize > 1)
    {
        for (int i = 1; i < ISIZE; ++i) 
        {
            for (int j = start_J; j < end_J; ++j)
            {
                localArray[j] = sin(2 * a[i - 1][j - 3]);
            }

             MPI_Allgatherv(&localArray[start_J], localArraySize, MPI_DOUBLE, &a[i][0],
                     &recvcnts[0], &displs[0], MPI_DOUBLE, MPI_COMM_WORLD);
        }
    } 
    else
    {
        for (int i = 1; i < ISIZE; ++i)
        {
            for (int j = 3; j < JSIZE; ++j)
            {
                a[i][j] = sin(2 * a[i - 1][j - 3]);
            }
        }
    }
//--------------------------

    MPI_Barrier(MPI_COMM_WORLD);
    double end;
    if (rank == 0)
    {
        end = MPI_Wtime();
        printf("Time spent: %lf\n", end - start);
    }


    // if (rank == 0)
    // {
    //     ff = fopen("result.txt","w");
    //     for(i=0; i < ISIZE; i++)
    //     {
    //         for (j=0; j < JSIZE; j++)
    //         {
    //             fprintf(ff,"%f ",a[i][j]);
    //         }
    //         fprintf(ff,"\n");
    //     }
    //     fclose(ff);
    // }

    for(int i = 0; i < ISIZE; ++i)
    {
        free(a[i]);
    }

    free(a);
    free(localArray);
    free(recvcnts);
    free(displs);

    MPI_Finalize();
}