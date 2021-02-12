#ifndef TSP_FUNC_H
#define TSP_FUNC_H

// 町の構造体（今回は2次元座標）を定義
typedef struct
{
    int x;
    int y;
} City;

// 描画用
typedef struct
{
    int width;
    int height;
    char **dot;
} Map;

// 解を収納
typedef struct answer
{
    int number;
    double sum_d;
    int *route;
} Answer;

// プロトタイプ宣言
// draw_line: 町の間を線で結ぶ
// draw_route: routeでの巡回順を元に移動経路を線で結ぶ
// plot_cities: 描画する
// distance: 2地点間の距離を計算
// solve(): TSPをといて距離を返す/ 引数route に巡回順を格納

//plot.c
Map init_map(const int width, const int height);
void free_map_dot(Map m);
void draw_line(Map map, City a, City b);
void draw_route(Map map, City *city, int n, const int *route);
void plot_cities(FILE *fp, Map map, City *city, int n, const int *route);
double distance(City a, City b);

//tsp.c
Answer *solve(const City *city, int n);
void search_HillClimb(const City *city, Answer *answer);
void search_SA(const City *city, Answer *answer);

//func.c
unsigned int randxor();
double distance(City a, City b);
Answer *init_answer(int n);
City *load_cities(const char *filename, int *n);
void calculate_sum_d(const City *city, Answer *answer);
void min(int n, double distance[n][n], int *i_best, int *j_best, double *d_best);
int max(const int a, const int b);
int check_exists(const int *route, const int a);
void swap(Answer *answer, int i, int j);
void swap_route(int *route, int i, int j);
double delta(const City *city, int *route, int i, int j, int n);
Answer *neighbor(const City *city, Answer *answer, double T);
Answer *copy_answer(Answer *oldanswer);
int find_index(const Answer *answer, const int a);
int restriction(Answer *answer);
void restrict_answer(Answer *answer);
void twoOPT(Answer *answer, int i, int j);
double temperature(long long k);
double probability(double deltaD, double T);

#endif
