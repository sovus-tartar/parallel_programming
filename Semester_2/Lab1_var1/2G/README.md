### Анализ цикла

a[i][j] = f(a[i+4][j-5])

a[0][5] = f(a[4][0])
a[0][6] = f(a[4][1])
a[0][7] = f(a[4][2])
a[0][8] = f(a[4][3])
a[0][9] = f(a[4][4])
a[0][10] = f(a[4][5])
a[0][11] = f(a[4][6])
...
a[1][5] = f(a[5][0])
a[1][6] = f(a[5][1])
a[1][7] = f(a[5][2])
a[1][8] = f(a[5][3])
a[1][9] = f(a[5][4])
a[1][10] = f(a[5][5])
a[1][11] = f(a[5][6])
...
a[2][5] = f(a[6][0])
a[2][6] = f(a[6][1])
a[2][7] = f(a[6][2])
a[2][8] = f(a[6][3])
a[2][9] = f(a[6][4])
a[2][10] = f(a[6][5])
a[2][11] = f(a[6][6])
...
a[3][5] = f(a[7][0])
a[3][6] = f(a[7][1])
a[3][7] = f(a[7][2])
a[3][8] = f(a[7][3])
a[3][9] = f(a[7][4])
a[3][10] = f(a[7][5])
a[3][11] = f(a[7][6])
...
a[4][5] = f(a[8][0])
a[4][6] = f(a[8][1])
a[4][7] = f(a[8][2])
a[4][8] = f(a[8][3])
a[4][9] = f(a[8][4])
a[4][10] = f(a[8][5])
a[4][11] = f(a[8][6])

Каждый следующий блок меняет данные, необходимые для предыдущего, поэтому необходимо параллелить в рамках каждого такого блока
i = 0-3, 4-6...
Внутри такого блока операции можно исполнять в любом порядке

В случае, если x >= y получается достаточно много накладных расходов, из-за чего ускорения нет или оно мало. Ускорение можно получить в случае y >> x

Результаты 10000х10000:
1 поток     - 0.4949s
2 потока    - 0.3174s
3 потока    - 0.2658s
4 потока    - 0.2207s
