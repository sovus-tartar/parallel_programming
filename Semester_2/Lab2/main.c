#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define N 1000
#define H 1 / (N - 1)

int NumberOfThreads = 1;
int NumberOfNewtonIterations = 5;

void gaussSolve(double** A, double* X, int n)
{
    int i, j, k;

    // Устанавливаем количество потоков для OpenMP
    omp_set_num_threads(NumberOfThreads);

    // Прямой ход метода Гаусса
    for (k = 0; k < n - 1; k++)
    {
        // Параллелизируем внутренние строки
        #pragma omp parallel for private(i, j) shared(A, k, n)
        for (i = k + 1; i < n; i++)
        {
            double factor = A[i][k] / A[k][k];
            // До n-2 так как проводим операции и с правой частью
            // #pragma omp parallel for private(j) shared(k, i, factor) - неэффективно
            for (j = k; j < n + 1; j++)
            {
                A[i][j] -= factor * A[k][j];
            }
        }
    }

    // Обратный ход метода Гаусса
    for (i = n - 1; i >= 0; i--)
    {
        X[i] = A[i][n] / A[i][i];

        #pragma omp parallel for private(j) shared(A, X, i, n)
        for (j = 0; j < i; j++)
        {
            A[j][n] -= A[j][i] * X[i];
        }
    }
}

void getMatrixYakobi(double** A, double* Y, int n)
{
    double h = 1.0 / (n - 1);

    for (int i = 0; i < n - 2; i++)
    {
        for (int j = 0; j < n - 2; j++)
        {
            A[i][j] = 0;
        }
    }

    // Захардкожены функции, ибо это на самом деле производные

    for (int i = 0; i < n - 2; i++)
    {
        A[i][i] = -2 - (10.0 / 12) * h * h * exp(Y[i + 1]);
    }

    for (int i = 0; i < n - 3; i++)
    {
        A[i][i + 1] = 1 - (h * h / 12) * exp(Y[i + 2]);
    }

    for (int i = 1; i < n - 2; i++)
    {
        A[i][i - 1] = 1 - (h * h / 12) * exp(Y[i]);
    }
}

void getBFromMatrixF(double* Y, int n, double* B)
{
    double h = 1.0 / (n - 1);

    for (int i = 0; i < n - 2; i++)
    {
        B[i] = -(Y[i + 2] - 2 * Y[i + 1] + Y[i] - h * h / 12 * (Y[i + 2] + 10 * Y[i + 1] + Y[i]));
    }
}

void NewtonLin(double *Y, int n)
{
    // A - расширенная матрица, у которой n-2 столбец - это столбец правой части СЛУ
    double** A = (double **)malloc((n - 2) * sizeof(double *));
    for (int i = 0; i < n - 2; i++)
    {
        A[i] = (double *)malloc((n - 1) * sizeof(double));
    }

    // Проводим NumberOfNewtonIterations итераций метода Ньютона
    for (int i = 0; i < NumberOfNewtonIterations; i++)
    {
        getMatrixYakobi(A, Y, n);
        double B[n - 2];
        getBFromMatrixF(Y, n, B);

        for (int j = 0; j < n - 2; j++)
        {
            A[j][n - 2] = B[j];
        }

        gaussSolve(A, B, n - 2);

        for (int j = 0; j < n - 2; j++)
        {
            Y[j + 1] += B[j];
        }
    }

    // Освобождаем память
    for (int i = 0; i < n - 2; i++)
    {
        free(A[i]);
    }
    free(A);
}

int main(int argc, char** argv)
{
    // Инициализируем количество потоков и количество итераций метода Ньютона
    NumberOfThreads = atoi(argv[1]);
    NumberOfNewtonIterations = atoi(argv[2]);

    // Выделяем память для матрицы и вектора решений

    double* Y = (double *)malloc(N * sizeof(double));
    
    // Задаём начальные условия
    Y[0] = 0;
    Y[N - 1] = 1;

    // Заполняем массив первым приближением
    for (int i = 0; i < N; i++)
    {
        Y[i] = Y[0] + i * (Y[N - 1] - Y[0]) / (N - 1);
    }

    double startTime = omp_get_wtime();
    NewtonLin(Y, N);
    double endTime = omp_get_wtime();

    double timeDiff = endTime - startTime;

    printf("Time: %f s\n", timeDiff);

    // Записываем Y в файл
    FILE *ff = fopen("result.txt", "w");
    for (int i = 0; i < N; i++)
    {
        fprintf(ff, "%f\n", Y[i]);
    }
    fclose(ff);

    // Освобождаем память
    free(Y);

    return 0;
}