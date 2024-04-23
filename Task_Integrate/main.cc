#include <stack>
#include <iostream>
#include <pthread.h>
#include <cmath>
// #include <boost/program_options.hpp>

#include <sys/select.h>
#include <unistd.h>
#include <sys/time.h>

#include <cstdlib>

struct Node_t
{
    double start;
    double end;

    bool isTerminal()
    {
        return (start > end);
    };

    double countAverage()
    {
        return (start + end) / 2.;
    };
};

// Секция с глобальными переменными/константами

constexpr int maxTask = 10000000;
int nProc = 8;
constexpr int SPK = 8;

double epsilon = 1e-14;

pthread_mutex_t globalStackMutex;
pthread_mutex_t globalStackTaskPresent;
std::stack<Node_t> globalStack;
volatile int nActive = 0;

pthread_mutex_t sumMutex;
volatile double globalSum = 0;

// Алгоритм взят из книги Якобовского "Введение в параллельные методы решения задач", страницы 269-273

bool breakCond(double sacb, double sab, double epsilon)
{
    return !(fabs(sacb - sab) < epsilon);
}

double function(double x)
{
    return cos(1. / x);
}

void* thread(void * ptr)
{
    std::stack<Node_t> localStack;
    double localSum = 0;

    // Начало цикла обработки стека интервалов
    while(1)
    {
        // чтение одного интервала из списка интервалов
        // Начало критической секции чтения из глобального
        // стека очередного интервала интегрирования

        pthread_mutex_lock(&globalStackTaskPresent);
        pthread_mutex_lock(&globalStackMutex);

        Node_t currInterval = globalStack.top();
        globalStack.pop();

        if(!globalStack.empty())
            pthread_mutex_unlock(&globalStackTaskPresent);

        if(!currInterval.isTerminal())
            nActive += 1;

        pthread_mutex_unlock(&globalStackMutex);

        // Конец критической секции чтения из глобального
        // стека очередного интервала интегрирования

        if(currInterval.isTerminal())
            break;

        while(1)
        {
            // std::cout << currInterval.start << " " << currInterval.end << std::endl;

            double c = currInterval.countAverage();
            double fc = function(c), fa = function(currInterval.start), fb = function(currInterval.end);
            double sac = (fa + fc) * (c - currInterval.start) / 2;
            double scb = (fc + fb) * (currInterval.end - c) / 2;
            double sacb = sac + scb;
            double sab = (fa + fb) * (currInterval.end - currInterval.start) / 2;

            if(!breakCond(sacb, sab, epsilon))
            {
                localSum += sacb;

                if(localStack.empty())
                    break;

                currInterval = localStack.top();
                localStack.pop();
            }
            else
            {
                Node_t tempNode = {currInterval.start, c};
                localStack.push(tempNode);
                currInterval = {c, currInterval.end};
            }

            // перемещение части локального стека
            // в общий список интервалов
            // Начало критической секции заполнения
            // глобального стека отрезками интегрирования


            if((localStack.size() > SPK) && (globalStack.empty()))
            {
                pthread_mutex_lock(&globalStackMutex);
                if(globalStack.empty())

                    // установить семафор наличия
                    // записей в глобальном стеке

                    pthread_mutex_unlock(&globalStackTaskPresent);

                while((!localStack.empty()) && (globalStack.size() < maxTask))
                {
                    Node_t tempNode = localStack.top();
                    localStack.pop();
                    globalStack.push(tempNode);
                }
                pthread_mutex_unlock(&globalStackMutex);
            }


            // Конец критической секции заполнения глобального
            // стека отрезками интегрирования
        }
        // конец цикла интегрирования одного интервала

        // Начало критической секции заполнения глобального
        // стека терминальными отрезками (a>b)

        pthread_mutex_lock(&globalStackMutex);
        nActive -= 1;

        if((!nActive) && (globalStack.empty()))
        {
            for(int i = 0; i < nProc; ++i)
            {
                Node_t tempNode = {2, 1};
                globalStack.push(tempNode);
            }
            // в глобальном стеке есть записи
            pthread_mutex_unlock(&globalStackTaskPresent);
        }


        pthread_mutex_unlock(&globalStackMutex);
        // Конец критической секции заполнения глобального
        // стека терминальными отрезками
    }
    // конец цикла обработки стека интервалов

    // Начало критической секции сложения частичных сумм
    pthread_mutex_lock(&sumMutex);
    globalSum += localSum;
    pthread_mutex_unlock(&sumMutex);
    // Конец критической секции сложения частичных сумм

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char ** argv)
{

    nProc = std::atoi(argv[1]);
    int powerExponent = std::atoi(argv[2]);

    epsilon = pow(10, -powerExponent);


    pthread_mutex_init(&globalStackMutex, NULL);
    pthread_mutex_unlock(&globalStackMutex);

    pthread_mutex_init(&globalStackTaskPresent, NULL);

    pthread_mutex_init(&sumMutex, NULL);
    pthread_mutex_unlock(&sumMutex);

    Node_t tempNode = {0.005, 4};
    globalStack.push(tempNode);

    pthread_mutex_unlock(&globalStackTaskPresent);

    pthread_t * threads = static_cast<pthread_t *>(malloc(nProc * sizeof(pthread_t)));

    struct timeval t1, t2;


    gettimeofday(&t1, NULL);

    for(int i = 0; i < nProc; ++i)
        pthread_create(threads + i, NULL, thread, NULL);

    for(int i = 0; i < nProc; ++i)
        pthread_join(threads[i], NULL);

    gettimeofday(&t2, NULL);

    long long deltaUsec = t2.tv_usec - t1.tv_usec;
    long long deltaSec = t2.tv_sec - t1.tv_sec;
    long long delta1 = deltaUsec + 1000000 * deltaSec;

    std::cout << "Значение интеграла S cos(1/x)dx от " << 5e-3 << " до " << 4 << " равно:\n" << globalSum << std::endl;
    std::cout << "Разница по времени: " << delta1 << " mks" << std::endl;
    std::cout << "Количество потоков: " << nProc << std::endl;

    free(threads);

    return 0;
}