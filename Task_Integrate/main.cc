#include <stack>
#include <iostream>
#include <pthread.h>

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

// Секция с глобальными переменными

constexpr int SPK = 8;
constexpr int maxTask = 8;
int nProc = 0;

pthread_mutex_t globalStackMutex;
pthread_mutex_t globalStackTaskPresent;
std::stack<Node_t> globalStack;
int nActive = 0;

pthread_mutex_t sumMutex;
int globalSum = 0;

// Алгоритм взят из книги Якобовского "Введение в параллельные методы решения задач", страницы 269-273

bool breakCond(double sacb, double sab, double epsilon)
{
    return !(abs(sacb - sab) < epsilon);
}

double function(double x)
{
    return {};
}

void* thread(void * ptr)
{
    std::stack<Node_t> localStack;
    double localSum = 0;

    double epsilon = 1e-6;

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
                currInterval = {c, currInterval.end};
            }

            // перемещение части локального стека
            // в общий список интервалов
            // Начало критической секции заполнения
            // глобального стека отрезками интегрирования

            pthread_mutex_lock(&globalStackMutex);
            if((localStack.size() > SPK) && (!globalStack.empty()))
            {
                if(!globalStack.empty())

                    // установить семафор наличия
                    // записей в глобальном стеке

                    pthread_mutex_unlock(&globalStackTaskPresent);

                while((!localStack.empty()) && (globalStack.size() < maxTask))
                {
                    Node_t tempNode = localStack.top();
                    localStack.pop();
                    globalStack.push(tempNode);
                }
            }
            pthread_mutex_unlock(&globalStackMutex);

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

    return NULL;
}

int main()
{

}