#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include "func.h"

double distance(City a, City b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

Answer *init_answer(int n)
{
    //領域を確保
    Answer *answer = (Answer *)malloc(sizeof(Answer));
    answer->number = n;
    // 訪れる順序を記録する配列を設定
    int *route = (int *)calloc(n, sizeof(int));
    int *already_generated = (int *)calloc(n, sizeof(int));
    answer->route = route;
    //初期解を生成。
    route[0] = 0; // 循環した結果を避けるため、常に0番目からスタート
    already_generated[0] = 1;
    for (int i = 1; i < n; i++)
    {
        int a;
        do
        {
            a = rand() % (n - 1) + 1; //1~n-1までの整数
        } while (already_generated[a] == 1);

        route[i] = a;
        already_generated[a] = 1;
    }
    free(already_generated);

    if (restriction(answer) != 1)
    {
        //2->1の順番になってたら、入れ替えればいい。
        //枝刈り条件として1->2の順番で必ず回らせるので、初期解からそうなるように適用。
        int r1 = find_index(answer, 1);
        int r2 = find_index(answer, 2);
        assert(r1 >= 0 && r2 >= 0);
        swap(answer, r1, r2);
    }

    return answer;
}

City *load_cities(const char *filename, int *n)
{
    City *city;
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s: cannot open file.\n", filename);
        exit(1);
    }
    fread(n, sizeof(int), 1, fp);
    city = (City *)malloc(sizeof(City) * *n);
    for (int i = 0; i < *n; i++)
    {
        fread(&city[i].x, sizeof(int), 1, fp);
        fread(&city[i].y, sizeof(int), 1, fp);
    }
    fclose(fp);
    return city;
}

int check_exists(const int *route, const int a)
{
    int flg = 0;
    for (int i = 0; i < sizeof(route) / sizeof(int); i++)
    {
        if (route[i] == a)
        {
            flg = 1;
            break;
        }
    }
    return flg;
}

int restriction(Answer *answer)
{
    //必ず2の前に1を通る。
    int n = answer->number;
    int *route = answer->route;

    int order1, order2;
    for (int i = 0; i < n; i++)
    {
        if (route[i] == 1)
        {
            order1 = i;
        }
        if (route[i] == 2)
        {
            order2 = i;
        }
    }

    int res = order1 < order2 ? 1 : 0;
    return res;
}

void min(int n, double distance[n][n], int *i_best, int *j_best, double *d_best)
{
    *i_best = -1;
    *j_best = -1;
    *d_best = INFINITY;
    for (int i = 1; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (distance[i][j] < *d_best)
            {
                *i_best = i;
                *j_best = j;
                *d_best = distance[i][j];
            }
        }
    }
}

// 整数最大値をとる関数
int max(const int a, const int b)
{
    return (a > b) ? a : b;
}

void swap(Answer *answer, int i, int j)
{
    int tmp = answer->route[i];
    answer->route[i] = answer->route[j];
    answer->route[j] = tmp;
}

int find_index(const Answer *answer, const int a)
{
    int res = -1;
    int n = answer->number;
    int *route = answer->route;
    for (int i = 0; i < n; i++)
    {
        if (route[i] == a)
        {
            res = i;
            break;
        }
    }
    return res;
}

void calculate_sum_d(const City *city, Answer *answer)
{
    int n = answer->number;
    int *route = answer->route;

    // トータルの巡回距離を計算する
    // 実際には再帰の末尾で計算することになる
    double sum_d = 0;
    for (int i = 0; i < n; i++)
    {
        const int c0 = route[i];
        const int c1 = route[(i + 1) % n]; // nは0に戻る
        sum_d += distance(city[c0], city[c1]);
    }
    answer->sum_d = sum_d;
}
