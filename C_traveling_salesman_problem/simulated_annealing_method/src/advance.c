#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include "func.h"
#include <time.h>

Answer *solve(const City *city, int n)
{
    int m = 1000; //試行回数
    Answer *answerlist[m];
    Answer *answer;

    time_t tSeed;
    tSeed = time((time_t *)0);
    srand((unsigned int)tSeed);
    //繰り返し試行
    for (int i = 0; i < m; i++)
    {
        answer = init_answer(n); //malloc領域に確保されてるので、こっちで改めてmallocする必要なし。
        calculate_sum_d(city, answer);
        // 局所最適解を探索する
        // search_HillClimb(city, answer); //山登り法
        search_SA(city, answer); //焼きなまし法
        //リストに保存する
        answerlist[i] = answer;
    }

    //最適解を抽出
    int min_index = -1;
    double min = INFINITY;
    for (int i = 0; i < m; i++)
    {
        if (answerlist[i]->sum_d < min)
        {
            min_index = i;
            min = answerlist[i]->sum_d;
        }
    }

    // 最適解以外はfreeする
    for (int i = 0; i < m; i++)
    {
        if (i != min_index)
        {
            free(answerlist[i]->route);
            free(answerlist[i]);
        }
    }

    return answerlist[min_index];
}

void search_SA(const City *city, Answer *startanswer)
{
    //都市数
    int n = startanswer->number;

    //探索中の解
    int *route = startanswer->route;
    double D = startanswer->sum_d;

    //暫定解
    Answer *bestanswer = copy_answer(startanswer);
    calculate_sum_d(city, bestanswer);
    int *bestroute = bestanswer->route;

    //暫定解の評価値;
    double bestD = D;

    long long max_i = n * n * 10;
    for (long long iter = 0; iter < max_i; iter++)
    {
        //温度
        double T = temperature(iter);

        //(bestroute)ではなくrouteに対して近傍をランダム生成する。
        int i = rand() % (n - 1) + 1;
        int j;
        do
        {
            j = rand() % (n - 1) + 1;
        } while (j == i);
        //まだ書き換えない

        //Dに対する差分を評価する。
        double deltaD = delta(city, route, i, j, n);

        if (D + deltaD < bestD)
        {
            //その通りに入れ替える
            swap_route(route, i, j);
            for (int k = 0; k < n; k++)
            {
                bestroute[k] = route[k];
            }
            bestD = D + deltaD;
            D = D + deltaD;

            continue;
        }

        if ((rand() / RAND_MAX) <= probability(deltaD, T) && deltaD < 3)
        {
            //その通りに入れ替えるように
            swap_route(route, i, j); //bestDは勝手に更新されてないので大丈夫。
            D = D + deltaD;
            continue;
        }
    }

    //改悪を許さないフェーズ
    max_i = n * n * 100;
    for (long long iter = 0; iter < max_i; iter++)
    {

        //(bestroute)ではなくrouteに対して近傍をランダム生成する。
        int i = rand() % (n - 1) + 1;
        int j;
        do
        {
            j = rand() % (n - 1) + 1;
        } while (j == i);
        //まだ書き換えない

        //Dに対する差分を評価する。
        double deltaD = delta(city, route, i, j, n);

        if (D + deltaD < bestD)
        {
            //その通りに入れ替える
            swap_route(route, i, j);
            for (int k = 0; k < n; k++)
            {
                bestroute[k] = route[k];
            }
            bestD = D + deltaD;
            D = D + deltaD;

            continue;
        }
    }

    //差し替える
    free(startanswer->route);
    startanswer->route = bestanswer->route;
    startanswer->sum_d = bestanswer->sum_d;
    free(bestanswer);

    calculate_sum_d(city, startanswer);
    return;
}

double temperature(long long k)
{
    double T0 = 100; //初期温度
    //k = index のi = 0->100000
    // //序盤はk小さい　k＝0～1

    //定義１
    double T = T0 / log2(k + 2);

    //定義2
    // double a = 0.99; //0～1。固定。
    // double T = T0 * pow(a, k);

    return T; //=a^r = 1->0
}

double probability(double deltaD, double T)
{
    if (deltaD < 0)
    {
        //改善されてるんだったらそもそもここにたどり着くはずないけど、一応
        return 1;
    }
    else
    {
        // 基本的にはd1(old)<d2(new)

        return exp(-deltaD / T);
    }
}
