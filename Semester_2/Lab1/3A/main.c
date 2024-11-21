#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include <mpi.h>

const uint8_t kMaster = 0;
const uint8_t kMsgTag = 0;
const uint8_t kReady = 1;
const uint8_t kNextToWrite = 0;


struct Margins
{
    int begin;
    int end;
};

struct Margins getAllocMargins(int rank, int size, int maxVal)
{
    struct Margins ret =
    {
        .begin = (maxVal / size) * rank,
        .end = (maxVal / size) * (rank + 1)
    };

    if (rank == (size - 1))
    {
        ret.end = maxVal;
    }
    else
    {
        ret.end = ret.end + 1;
    }

    return ret;
}

void mpiWait(int rank, int size)
{
    if (rank == kMaster)
    {
        for (int i = 1; i < size - 1; ++i)
        {
            int temp;
            MPI_Recv(&temp, 1, MPI_INT, i, kReady, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        int temp = 0;
        MPI_Send(&temp, 1, MPI_INT, kMaster, kReady, MPI_COMM_WORLD);
    }
}

void waitForTurn(int rank, int size)
{
    if (rank == kMaster)
    {
        return;
    }
    else
    {
        int temp;
        MPI_Recv(&temp, 1, MPI_INT, rank - 1, kNextToWrite, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

void nextInTurn(int rank, int size)
{
    if (rank == (size - 1))
    {
        return;
    }
    else
    {
        int temp;
        MPI_Send(&temp, 1, MPI_INT, rank + 1, kNextToWrite, MPI_COMM_WORLD);
    }
}

void writeToFile(int rank, int size, double** a, const struct Margins X, const struct Margins Y)
{
    char flags[3] = "";

    if (rank == kMaster)
    {
        strcpy(flags, "w");
    }
    else
    {
        strcpy(flags, "a");
    }

    waitForTurn(rank, size);
    // printf("%d is starting writing\n", rank);
    FILE *ff;
    ff = fopen("result.txt", flags);

    int x_end = (rank == (size - 1)) ? X.end : X.end - 1;

    for(int i = X.begin; i < x_end; i++)
    {
        for (int j = Y.begin; j < Y.end; j++)
        {
            fprintf(ff,"%f ",a[i - X.begin][j - Y.begin]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);
    // printf("%d has finished starting writing\n", rank);
    nextInTurn(rank, size);
    
}

// #define f(x) sin((0.1) * (x))
// #define g(x) (x) * (1.5)

void executer(int rank, int size, double** a, double** b, const struct Margins X, const struct Margins Y)
{
    mpiWait(rank, size);

    double start;
    if (rank == kMaster)
    {
        start = MPI_Wtime();
    }

    // требуется обеспечить измерение времени работы данного цикла
    for (int i = X.begin; i < X.end - 1; i++)
    {
        for (int j = Y.begin; j < Y.end; j++)
        {
            b[i - X.begin][j - Y.begin] = 1.5 * sin(0.1 * a[i - X.begin + 1][j - Y.begin]);
        }
    }

    mpiWait(rank, size);

    double stop;
    if (rank == kMaster)
    {
        stop = MPI_Wtime();
        printf("Time spent: %lf sec\n", (stop - start));
    }

#ifndef DISABLE_OUTPUT
    writeToFile(rank, size, b, X, Y);
#endif

}

void handleCallocError(void * ptr)
{
    if (ptr == NULL)
    {
        printf("Calloc failed to allocate memory, aborting...\n");
        abort();
    }
}

int getMarginSize(struct Margins margin)
{
    return margin.end - margin.begin;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    const int x = atoi(argv[1]);
    const int y = atoi(argv[2]);

    int rank = 0;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    struct Margins X = getAllocMargins(rank, size, x);
    struct Margins Y = {0, y};

    double** a = (double **)calloc(getMarginSize(X), sizeof(double**));
    handleCallocError(a);
    for(int i = X.begin; i < X.end; ++i)
    {
        a[i - X.begin] = (double*)calloc(getMarginSize(Y), sizeof(double*));
        handleCallocError(a[i - X.begin]);
    }

    double** b = (double **)calloc(getMarginSize(X), sizeof(double**));
    handleCallocError(b);
    for(int i = X.begin; i < X.end; ++i)
    {
        b[i - X.begin] = (double*)calloc(getMarginSize(Y), sizeof(double*));
        handleCallocError(b[i - X.begin]);
    }

    FILE *ff;
    //подготовительная часть – заполнение некими данными
    for (int i = X.begin; i < X.end; i++)
    {
        for (int j = Y.begin; j < Y.end; j++)
        {
            a[i - X.begin][j - Y.begin] = 10 * i + j;
        }
    }

    executer(rank, size, a, b, X, Y);

    for(int i = X.begin; i < X.end; ++i)
    {
        free(a[i - X.begin]);
        free(b[i - X.begin]);

    }
    free(a);
    free(b);


    MPI_Finalize();
}