#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include "func.h"

Answer *solve(const City *city, int n)
{
    int m = 100; //試行回数
    Answer *answerlist[m];
    Answer *answer;

    //繰り返し試行
    for (int i = 0; i < m; i++)
    {
        answer = init_answer(n); //malloc領域に確保されてるので、こっちで改めてmallocする必要なし。

        // 局所最適解を探索する
        search(city, answer);
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

void search(const City *city, Answer *answer)
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
        search(city, answer);
    }

    //変わらなかったら、終了。
    return;
}
