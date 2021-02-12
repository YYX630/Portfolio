#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h> // strtol のエラー判定用
#include <time.h>
#include "func.h"

int main(int argc, char **argv)
{
  // const による定数定義
  const int width = 70;
  const int height = 40;
  const int max_cities = 100;

  Map map = init_map(width, height);

  FILE *fp = stdout; // とりあえず描画先は標準出力としておく
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <city file>\n", argv[0]);
    exit(1);
  }
  int n;

  City *city = load_cities(argv[1], &n);
  assert(n > 1 && n <= max_cities); // さすがに都市数100は厳しいので

  // 町の初期配置を表示
  plot_cities(fp, map, city, n, NULL);
  sleep(1);

  //解く
  Answer *answer = solve(city, n);

  //表示する
  const int *route = answer->route;
  const double d = answer->sum_d;
  plot_cities(fp, map, city, answer->number, answer->route);
  printf("total distance = %f\n", d);
  for (int i = 0; i < n; i++)
  {
    printf("%d -> ", route[i]);
  }
  printf("0\n");

  // 動的確保した環境ではfreeをする
  free(answer->route);
  free(answer);
  free(city);

  return 0;
}
