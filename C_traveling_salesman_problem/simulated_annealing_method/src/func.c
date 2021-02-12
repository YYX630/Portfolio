#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include "func.h"

unsigned int randxor()
{
    static unsigned int x = 123456789, y = 362436069, z = 521288629, w = 88675123;
    unsigned int t;
    t = (x ^ (x << 11));
    x = y;
    y = z;
    z = w;
    return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
}

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

    // 1->2の順番に制限
    // restrict_answer(answer);
    //焼きなましでは必要ないかな。

    return answer;
}

void restrict_answer(Answer *answer)
{
    if (restriction(answer) != 1)
    {
        //2->1の順番になってたら、入れ替えればいい。
        //枝刈り条件として1->2の順番で必ず回らせるので、初期解からそうなるように適用。
        int r1 = find_index(answer, 1);
        int r2 = find_index(answer, 2);
        assert(r1 >= 0 && r2 >= 0);
        swap(answer, r1, r2);
    }
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
// Answer *neighbor(const City *city, Answer *answer, double r)
// {
//     int n = answer->number;
//     //ランダムに近傍を生成する。
//     //領域を確保
//     Answer *nextanswer = (Answer *)malloc(sizeof(Answer));
//     // 訪れる順序を記録する配列を設定
//     int *route = (int *)calloc(n, sizeof(int));
//     //コピー
//     for (int i = 0; i < n; i++)
//     {
//         route[i] = answer->route[i];
//     }

//     nextanswer->route = route;
//     nextanswer->number = n;

//     //近傍生成の手法が一番大事っぽいな。
//     //序盤は大きく変化、終盤は小さく変化させる
//     //rは０－＞１で線形で動く
//     int iter;
//     if (r < 0.1)
//         iter = 10;
//     else
//         iter = 1;
//     for (int k = 0; k < iter; k++)
//     {
//         int i = rand() % (n - 1) + 1;
//         int j;
//         do
//         {
//             j = rand() % (n - 1) + 1;
//         } while (j == i);
//         // twoOPT(nextanswer, i, j);
//         swap(nextanswer, i, j); //シンプルに？
//     }

//     calculate_sum_d(city, nextanswer);
//     return nextanswer;
// }

void search_HillClimb(const City *city, Answer *answer)
{
    //初期解はすでに生成されている。
    //2都市の入れ替えパターンのうち最短になるもので更新。
    //それ以上よくならなくなったら再帰終了。
    int n = answer->number;

    //距離の配列
    double distance[n][n];

    //オリジナル距離
    calculate_sum_d(city, answer);
    double d_original = answer->sum_d;

    //近傍調べる
    //注意！スタート０は固定だからswapしちゃだめ
    for (int i = 1; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            //置き換える。
            swap(answer, i, j);

            //ただし、順番制約をみたさなくなる置き換えは許さない
            // 枝刈り制約＝特定の2都市の順番を指定する。ここでは1->2
            if (restriction(answer) == 1)
            {
                //置換後の距離を調べる。
                calculate_sum_d(city, answer);
                distance[i][j] = answer->sum_d;
            }
            else
            {
                distance[i][j] = INFINITY; //間違っても最適解にならないようにする
            }

            //次のループに進む前に戻す
            swap(answer, i, j);
        }
    }

    //最も小さいやつを見つける。
    int i_best = -1;
    int j_best = -1;
    double d_best = 1000;
    min(n, distance, &i_best, &j_best, &d_best);
    if (d_original > d_best)
    {
        //よくなったら、そのように置き換えて、再捜査。
        swap(answer, i_best, j_best);
        assert(restriction(answer) == 1);
        search_HillClimb(city, answer);
    }

    //変わらなかったら、終了。
    return;
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

Answer *copy_answer(Answer *oldanswer)
{
    int n = oldanswer->number;

    Answer *newanswer = malloc(sizeof(Answer));
    int *newroute = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
    {
        newroute[i] = oldanswer->route[i];
    }
    newanswer->route = newroute;
    newanswer->number = n;
    newanswer->sum_d = oldanswer->sum_d;

    return newanswer;
}

double delta(const City *city, int *route, int i, int j, int n)
{
    // if (i > j)
    // {
    //     int tmp = i;
    //     i = j;
    //     j = tmp;
    // }
    int iminus = i - 1 >= 0 ? i - 1 : n;
    int iplus = i + 1 <= n ? i + 1 : 0;
    int jminus = j - 1 >= 0 ? j - 1 : n;
    int jplus = j + 1 <= n ? j + 1 : 0;

    double before = distance(city[route[iminus]], city[route[i]]) + distance(city[route[i]], city[route[iplus]]) + distance(city[route[jminus]], city[route[j]]) + distance(city[route[j]], city[route[jplus]]);

    double after = distance(city[route[iminus]], city[route[j]]) + distance(city[route[j]], city[route[iplus]]) + distance(city[route[jminus]], city[route[i]]) + distance(city[route[i]], city[route[jplus]]);

    double delta = after - before;

    return delta;
}

void twoOPT(Answer *answer, int i, int j)
{

    //山登りほうでは訪れる順番の中で二点を交代させた。
    //ここでは二辺を交換する2-opt法でいく?
    //ランダムな２数で、2辺を指定。(辺の起点が定まれば終点も定まる)
    int n = answer->number;
    int *route = answer->route;
    int I, J; //I<J
    if (i < j)
    {
        I = i;
        J = j;
    }
    else
    {
        I = j;
        J = i;
    }
    //とにかく、先に訪れる都市がroute[I]であとがroute[J]
    int *newroute = calloc(n, sizeof(int));
    for (int k = 0; k < n; k++)
    {
        if (k <= I || k > J)
        {
            newroute[k] = route[k];
        }
        else if (I < k && k <= J)
        {
            newroute[k] = route[J - (k - I - 1)];
        }
    }
    free(answer->route);
    answer->route = newroute;
}

void swap(Answer *answer, int i, int j)
{
    swap_route(answer->route, i, j);
}

void swap_route(int *route, int i, int j)
{
    int tmp = route[i];
    route[i] = route[j];
    route[j] = tmp;
}
