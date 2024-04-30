// Для выполнения задания была выбрана разностная схема "прямоугольник"
//
// (u[p+1][m-1] - u[p][m-1] + u[p+1][m] - u[p][m]) / 2tau + (u[p+1][m] - u[p+1][m-1] + u[p][m] - u[p][m-1])/2h = f[p+1/2][m-1/2]
//
// Явный вид:
// u[p+1][m+1] = (2tau * h / (h + tau)) * f[p+1/2][m-1/2] + ((tau - h)/(tau + h)) * (u[p+1][m] - u[p][m] - u[p][m+1])
// p - строки, m - столбцы
//
// ^ x  |*| ---> |!|                    upleft          upright
// |             ^                      
// |            / |
// |           /  |
// |          /   |
// |         /    |
// |        /     |
// |       /      |
// |      /       |
// |     |*|      |*|                   downleft        downright
// * ---------------------> t
// Сетка будет поделена на nproc столбцов, вычисления в каждом из которых будут проводиться параллельно

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

// t [0; T]
// x [0; X]
const double PI     = 3.1415;

const double tau    = 1e-1;
const double h      = 1e-1;

const double T      = 10;
const double X      = 10;

// M строк, K столбцов
const int K      = T / tau;
const int M      = X / h;  


double phi(double x) 
{
    return cos(PI * x);
}

double psi(double t) 
{
    return exp(-t);
}


double f(double x, double t) 
{
    return x + t;
}

struct executerInfo
{
    int start;
    int end;
};



void executer(int commsize, int rank, double ** gridArray, struct executerInfo info)
{
    for(int i = 1; i < M; ++i)
    {
        double upleft, downleft, downright;

        if(rank != 0)
        {
            double temp[2];
            MPI_Recv(temp, 2, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("process %d recieved %lf\n", rank, upleft);
            upleft = temp[0];
            downleft = temp[1];
        }
        else 
        {
            upleft = gridArray[i][info.start];
            downleft = gridArray[i - 1][info.start + 0];
        }


        downright = gridArray[i - 1][info.start + 1];

        for(int j = info.start + 1; j <= info.end; ++j)
        {

            gridArray[i][j] = ((2 * tau * h) / (h + tau)) * f((i + 1./2.) * h, (j - 1./2.) * tau) + downleft + (upleft - downright) * ((tau - h) / (tau + h));

            // printf("----------------------------\ni = %d,\tj = %d\nupleft = %.3lf,\tupright = %.3lf\ndownleft = %.3lf,\tdownright = %.3lf\n", i, j, upleft, gridArray[i][j], downleft, downright);
            // printf("rank = %d i = %d j = %d ul = %lf ur = %lf dl = %lf dr = %lf\n", rank, i, j, upleft, gridArray[i][j], downleft, downright);

            if(j != info.end)
            {
                upleft      = gridArray[i][j];
                downleft    = gridArray[i-1][j];
                downright   = gridArray[i-1][j+1];
            }
            
        }

        if(rank < (commsize - 1))
        {
            double temp[2] = {gridArray[i][info.end], gridArray[i-1][info.end]};
            // printf("process %d sending %lf\n", rank, gridArray[i][info.end]);
            MPI_Send(temp, 2, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
            
    }

    // starting sending out
#ifndef DISABLE_OUTPUT
    if(!rank)
    {
        printf("t\tx\tu\n");

        for(int i = 0; i < M; ++i)
            for(int j = info.start; j <= info.end; ++j)
                printf("%.6f\t%.6f\t%.6f\n", tau * i, h * j, gridArray[i][j]);

        int temp = 1;
        if(commsize > 1)
            MPI_Send(&temp, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
    }
    else // rank != 0
    {
        int temp;
        MPI_Recv(&temp, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // printf("process %d starts to out\n", rank);

        for(int i = 0; i < M; ++i)
            for(int j = info.start + 1; j <= info.end; ++j)
            {
                // printf("hello\n");
                printf("%.6f\t%.6f\t%.6f\n", tau * i, h * j, gridArray[i][j]);
            }
                
        // printf("start %d end %d\n", info.start, info.end);
        // printf("process %d ends to out\n", rank);
        
        if(rank < (commsize - 1))
            MPI_Send(&temp, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
    }
# endif
        
}

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    
    int commsize, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    double **gridArray = (double **) calloc(M, sizeof(double *));
    assert(gridArray);

    for(int i = 0; i < M; ++i)
    {
        gridArray[i] = (double *)calloc(K, sizeof(double));
        assert(gridArray[i]);
    }

    // TODO: Заполнить начальными условиями

    for(int j = 0; j < K; ++j)
        gridArray[0][j] = phi(j * h);
    for(int i = 0; i < M; ++i)
        gridArray[i][0] = psi(i * tau);

    struct executerInfo * infoArr = (struct executerInfo *) calloc(commsize, sizeof(struct executerInfo));
    if(commsize > 1)
    {
        infoArr[0].start = 0;
        infoArr[0].end = K / commsize;

        for(int i = 1; i < commsize; ++i)
        {
            if(i != (commsize - 1))
            {
                infoArr[i].start = infoArr[i - 1].end;
                infoArr[i].end = infoArr[i].start + K / commsize;
            }
            else 
            {
                infoArr[i].start = infoArr[i - 1].end;
                infoArr[i].end = K - 1;
            }
        }
    }
    else 
    {
        infoArr[0].start = 0;
        infoArr[0].end = K - 1;
    }

    
    // printf("rank = %dstart = %d, end = %d\n", rank, infoArr[rank].start, infoArr[rank].end);

    executer(commsize, rank, gridArray, infoArr[rank]);

    free(infoArr);

    if(rank == 0) 
    {
        // printf("t\tx\tu\n");

        // for(int i = 0; i < M; ++i)
        //     for(int j = 0; j < K; ++j)
        //         printf("%.6f\t%.6f\t%.6f\n", tau * i, h * j, gridArray[i][j]);

        for(int i = 0; i < M; ++i)
            free(gridArray[i]);

        free(gridArray);
    }
    
    MPI_Finalize();

    return 0;
}